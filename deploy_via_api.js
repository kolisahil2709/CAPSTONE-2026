const fs = require('fs');
const path = require('path');
const os = require('os');
const crypto = require('crypto');
const zlib = require('zlib');

function getSha256(buffer) {
  return crypto.createHash('sha256').update(buffer).digest('hex');
}

async function deployToFirebaseHosting() {
  console.log('====================================================');
  console.log('  CHRONOS MIS: DIRECT FIREBASE HOSTING DEPLOYER     ');
  console.log('====================================================');

  // 1. Get access token
  console.log('1. Obtaining OAuth Access Token...');
  const cfgPath = path.join(os.homedir(), '.config', 'configstore', 'firebase-tools.json');
  const cfg = JSON.parse(fs.readFileSync(cfgPath, 'utf8'));
  const rt = cfg.tokens.refresh_token;

  const auth = require(path.join(process.env.APPDATA, 'npm', 'node_modules', 'firebase-tools', 'lib', 'auth.js'));
  const authRes = await auth.getAccessToken(rt, []);
  const token = authRes.access_token;
  console.log('✓ Authenticated as:', cfg.user ? cfg.user.email : 'Google Account');

  // 2. Prepare files to deploy
  console.log('2. Preparing files & calculating SHA256 hashes...');
  const filesToDeploy = {};
  const fileBuffers = {};

  const fileList = [
    { disk: 'index.html', urlPath: '/index.html' },
    { disk: 'cloud_erp.html', urlPath: '/cloud_erp.html' }
  ];

  for (const item of fileList) {
    const filePath = path.join(__dirname, item.disk);
    if (!fs.existsSync(filePath)) continue;
    const rawBuf = fs.readFileSync(filePath);
    // Gzip compress for Firebase Hosting
    const gzipped = zlib.gzipSync(rawBuf, { level: 9 });
    const hash = getSha256(gzipped);
    filesToDeploy[item.urlPath] = hash;
    fileBuffers[hash] = gzipped;
    console.log(`   ${item.urlPath}: ${rawBuf.length} bytes -> ${gzipped.length} bytes (gzipped), hash: ${hash.substring(0, 12)}...`);
  }

  // 3. Create new Hosting Version
  console.log('3. Creating new Hosting Version for site chronosmis...');
  const createVersionRes = await fetch('https://firebasehosting.googleapis.com/v1beta1/sites/chronosmis/versions', {
    method: 'POST',
    headers: {
      Authorization: `Bearer ${token}`,
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      config: {
        rewrites: [
          { glob: '**', path: '/index.html' }
        ],
        headers: [
          {
            glob: '**',
            headers: {
              'Access-Control-Allow-Origin': '*',
              'Cache-Control': 'no-cache, no-store, must-revalidate'
            }
          }
        ]
      }
    })
  });

  if (!createVersionRes.ok) {
    const errText = await createVersionRes.text();
    throw new Error(`Failed to create version: ${createVersionRes.status} ${errText}`);
  }

  const versionData = await createVersionRes.json();
  const versionName = versionData.name; // e.g. sites/chronosmis/versions/12345
  console.log('✓ Created Version:', versionName);

  // 4. Populate Files
  console.log('4. Populating files to version...');
  const popRes = await fetch(`https://firebasehosting.googleapis.com/v1beta1/${versionName}:populateFiles`, {
    method: 'POST',
    headers: {
      Authorization: `Bearer ${token}`,
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      files: filesToDeploy
    })
  });

  if (!popRes.ok) {
    const errText = await popRes.text();
    throw new Error(`Failed to populate files: ${popRes.status} ${errText}`);
  }

  const popData = await popRes.json();
  const uploadUrl = popData.uploadUrl;
  const requiredHashes = popData.uploadRequiredHashes || [];
  console.log(`✓ Populate complete. Required uploads: ${requiredHashes.length} files`);

  // 5. Upload files
  for (const hash of requiredHashes) {
    console.log(`   Uploading blob ${hash.substring(0, 12)}...`);
    const uploadRes = await fetch(`${uploadUrl}/${hash}`, {
      method: 'POST',
      headers: {
        Authorization: `Bearer ${token}`,
        'Content-Type': 'application/octet-stream'
      },
      body: fileBuffers[hash]
    });
    if (!uploadRes.ok) {
      throw new Error(`Failed to upload ${hash}: ${uploadRes.status}`);
    }
  }
  console.log('✓ All file blobs uploaded successfully');

  // 6. Finalize Version (PATCH status: FINALIZED)
  console.log('5. Finalizing version...');
  const finalizeRes = await fetch(`https://firebasehosting.googleapis.com/v1beta1/${versionName}?update_mask=status`, {
    method: 'PATCH',
    headers: {
      Authorization: `Bearer ${token}`,
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      status: 'FINALIZED'
    })
  });

  if (!finalizeRes.ok) {
    const errText = await finalizeRes.text();
    throw new Error(`Failed to finalize version: ${finalizeRes.status} ${errText}`);
  }
  console.log('✓ Version finalized successfully');

  // 7. Release Version to Production!
  console.log('6. Releasing version to production (https://chronosmis.web.app)...');
  const releaseRes = await fetch(`https://firebasehosting.googleapis.com/v1beta1/sites/chronosmis/releases?versionName=${versionName}`, {
    method: 'POST',
    headers: {
      Authorization: `Bearer ${token}`,
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({
      message: 'Automated release of full-size Configuration view & linear styling'
    })
  });

  if (!releaseRes.ok) {
    const errText = await releaseRes.text();
    throw new Error(`Failed to create release: ${releaseRes.status} ${errText}`);
  }

  const releaseData = await releaseRes.json();
  console.log('✓ RELEASED TO PRODUCTION! Release ID:', releaseData.name);
  console.log('\n====================================================');
  console.log('  DEPLOYMENT SUCCESSFUL!                            ');
  console.log('  Live URL: https://chronosmis.web.app/             ');
  console.log('====================================================');
}

deployToFirebaseHosting().catch(err => {
  console.error('\n❌ DEPLOYMENT FAILED:', err.message);
  process.exit(1);
});
