const { spawn } = require('child_process');
const path = require('path');

const npmPath = path.join(process.env.APPDATA, 'npm');
const env = { ...process.env, PATH: npmPath + path.delimiter + process.env.PATH };

console.log('Deploying to Firebase Hosting for project chronosmis...');
const proc = spawn('cmd.exe', ['/c', 'firebase', 'deploy', '--only', 'hosting', '-P', 'chronosmis'], { env });

proc.stdout.on('data', d => process.stdout.write(d.toString()));
proc.stderr.on('data', d => process.stderr.write(d.toString()));

proc.on('close', code => {
  console.log('\nDeployment process exited with code:', code);
});
