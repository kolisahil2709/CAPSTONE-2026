const fs = require('fs');

const content = fs.readFileSync('index.html', 'utf8');
const lines = content.split('\n');
lines.forEach((line, idx) => {
  if (line.includes('kpi-terminal-status') || line.includes('terminal-status') || line.includes('updateKpis') || line.includes('pollDevice') || line.includes('checkDevice')) {
    console.log((idx + 1) + ': ' + line.trim());
  }
});
