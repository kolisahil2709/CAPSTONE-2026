const fs = require('fs');

const code = fs.readFileSync('index.html', 'utf8');
const scriptMatches = code.match(/<script[\s\S]*?>([\s\S]*?)<\/script>/gi);
console.log('Script count:', scriptMatches ? scriptMatches.length : 0);
scriptMatches.forEach((s, idx) => {
  const rawJs = s.replace(/<script[\s\S]*?>/i, '').replace(/<\/script>/i, '');
  new Function(rawJs);
  console.log('Script syntax verified OK!');
});
