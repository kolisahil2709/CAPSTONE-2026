const sqlite3 = require('sqlite3').verbose();
const path = require('path');
const dbPath = path.join(__dirname, 'capstone.db');

const db = new sqlite3.Database(dbPath, (err) => {
  if (err) {
    return console.error('❌ Error opening database:', err.message);
  }
  
  // 1. Query Employees Table
  db.all('SELECT * FROM employees', [], (err, employees) => {
    if (err) {
      console.error('❌ Error querying employees:', err.message);
      db.close();
      return;
    }
    
    console.log('\n👥 ======================= SYNCED EMPLOYEE PROFILES =======================');
    if (employees.length === 0) {
      console.log('   (No employees synced yet. Make sure your Webhook URL is set in ESP32 settings)');
    } else {
      console.table(employees);
    }
    
    // 2. Query Attendance Logs Table
    db.all('SELECT * FROM attendance_logs', [], (err, logs) => {
      if (err) {
        console.error('❌ Error querying logs:', err.message);
        db.close();
        return;
      }
      
      console.log('\n📊 ======================= ATTENDANCE SWIPE LOGS =======================');
      if (logs.length === 0) {
        console.log('   (No swipes recorded yet. Click "In" or "Out" next to an employee to test)');
      } else {
        console.table(logs);
      }
      console.log('=========================================================================\n');
      
      db.close();
    });
  });
});
