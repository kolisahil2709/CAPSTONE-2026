const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');
const app = express();

app.use(express.json()); // Enable JSON body parsing

// Enable CORS for all routes (so dashboard page on ESP32 can sync data)
app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Headers', 'Origin, X-Requested-With, Content-Type, Accept');
  res.header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  if (req.method === 'OPTIONS') {
    return res.sendStatus(200);
  }
  next();
});

// ==========================================
// 1. DATABASE CONFIGURATION (SQLite)
// Automatically creates a local file named "capstone.db"
// ==========================================
const dbPath = path.join(__dirname, 'capstone.db');
const db = new sqlite3.Database(dbPath, (err) => {
  if (err) {
    console.error('❌ Database connection failed! Error:', err.message);
    return;
  }
  console.log('✅ Connected to SQLite Database successfully!');
  
  // Auto-initialize: Create the attendance_logs table
  db.run(`CREATE TABLE IF NOT EXISTS attendance_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    uid TEXT NOT NULL,
    employee_name TEXT NOT NULL,
    role TEXT NOT NULL,
    direction TEXT NOT NULL, -- "In" or "Out"
    status TEXT NOT NULL,    -- "On-Time", "Late", "Early Exit", "Denied", "Accepted"
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
  )`, (tableErr) => {
    if (tableErr) console.error('❌ Failed to initialize attendance_logs table:', tableErr.message);
    else console.log('📋 Attendance logs table is ready!');
  });

  // Auto-initialize: Create the employees table for profile backup
  db.run(`CREATE TABLE IF NOT EXISTS employees (
    uid TEXT PRIMARY KEY,
    employee_name TEXT NOT NULL,
    role TEXT NOT NULL,
    roll TEXT NOT NULL,
    dept TEXT NOT NULL,
    type TEXT NOT NULL,      -- "RFID" or "FINGER"
    rfid TEXT                -- Linked RFID UID if type is FINGER
  )`, (tableErr) => {
    if (tableErr) console.error('❌ Failed to initialize employees table:', tableErr.message);
    else console.log('📋 Employees profile sync table is ready!');
  });
});

// ==========================================
// 2. ATTENDANCE LOG WEBHOOK ENDPOINT
// ==========================================
app.post('/webhook/scan', (req, res) => {
  console.log('\n📥 Received data from ESP32:', req.body);
  const { uid, name, role, dir, status } = req.body;

  if (!uid) {
    console.warn('⚠️ Rejected request: Missing UID');
    return res.status(400).send({ error: 'Missing UID in payload' });
  }

  const query = 'INSERT INTO attendance_logs (uid, employee_name, role, direction, status) VALUES (?, ?, ?, ?, ?)';
  const values = [
    uid,
    name || 'Unknown',
    role || '-',
    dir || 'In',
    status || 'Accepted'
  ];

  db.run(query, values, function(err) {
    if (err) {
      console.error('❌ Error inserting log into database:', err.message);
      return res.status(500).send({ error: 'Database insertion failed', details: err.message });
    }
    console.log(`💾 Saved Log to SQLite! Log ID: ${this.lastID} | Name: ${name || 'Unknown'} | Dir: ${dir || 'In'} | Status: ${status || 'Accepted'}`);
    res.status(200).send({ success: true, logId: this.lastID });
  });
});

// ==========================================
// 3. EMPLOYEE SYNC ENDPOINTS
// ==========================================

// Add or edit a single employee profile
app.post('/api/sync-employee', (req, res) => {
  console.log('🔄 Syncing employee profile:', req.body);
  const { uid, name, role, roll, dept, type, rfid } = req.body;

  if (!uid || !name) {
    return res.status(400).send({ error: 'Missing UID or Name in payload' });
  }

  // SQLite upsert syntax (INSERT OR REPLACE)
  const query = `INSERT INTO employees (uid, employee_name, role, roll, dept, type, rfid) 
                 VALUES (?, ?, ?, ?, ?, ?, ?) 
                 ON CONFLICT(uid) DO UPDATE SET 
                   employee_name = excluded.employee_name,
                   role = excluded.role,
                   roll = excluded.roll,
                   dept = excluded.dept,
                   type = excluded.type,
                   rfid = excluded.rfid`;

  const values = [uid, name, role || '-', roll || '', dept || '-', type || 'RFID', rfid || ''];

  db.run(query, values, function(err) {
    if (err) {
      console.error('❌ Error syncing profile to database:', err.message);
      return res.status(500).send({ error: 'Profile sync failed', details: err.message });
    }
    console.log(`💾 Profile synced! Name: ${name} | UID: ${uid}`);
    res.status(200).send({ success: true });
  });
});

// Delete an employee profile
app.post('/api/delete-employee', (req, res) => {
  console.log('🗑 Delete request for employee:', req.body);
  const { uid } = req.body;

  if (!uid) {
    return res.status(400).send({ error: 'Missing UID' });
  }

  const query = 'DELETE FROM employees WHERE uid = ?';
  db.run(query, [uid], function(err) {
    if (err) {
      console.error('❌ Error deleting profile:', err.message);
      return res.status(500).send({ error: 'Deletion failed', details: err.message });
    }
    console.log(`🗑 Deleted profile from SQLite! UID: ${uid}`);
    res.status(200).send({ success: true });
  });
});

// Sync all employees (Bulk import)
app.post('/api/sync-all-employees', (req, res) => {
  console.log('🔄 Bulk syncing all employee profiles...');
  const users = req.body; // Array of users

  if (!Array.isArray(users)) {
    return res.status(400).send({ error: 'Payload must be a JSON array of users' });
  }

  // Use a transaction for fast bulk insert
  db.serialize(() => {
    db.run('BEGIN TRANSACTION');
    db.run('DELETE FROM employees'); // Clear current cache
    
    const stmt = db.prepare('INSERT INTO employees (uid, employee_name, role, roll, dept, type, rfid) VALUES (?, ?, ?, ?, ?, ?, ?)');
    
    users.forEach(u => {
      stmt.run(u.uid, u.name, u.role || '-', u.roll || '', u.dept || '-', u.type || 'RFID', u.rfid || '');
    });

    stmt.finalize();
    db.run('COMMIT', (err) => {
      if (err) {
        console.error('❌ Bulk sync failed:', err.message);
        return res.status(500).send({ error: 'Bulk sync failed', details: err.message });
      }
      console.log(`💾 Bulk sync complete! Saved ${users.length} profiles to SQLite.`);
      res.status(200).send({ success: true, count: users.length });
    });
  });
});

// Sync all attendance logs (Bulk import with duplicate checking)
app.post('/api/sync-logs', async (req, res) => {
  console.log('\n🔄 Received attendance logs to sync:', req.body.length);
  const logs = req.body;
  if (!Array.isArray(logs)) {
    return res.status(400).send({ error: 'Payload must be a JSON array of logs' });
  }

  let inserted = 0;
  let skipped = 0;

  try {
    // Begin SQLite transaction
    await new Promise((resolve, reject) => {
      db.serialize(() => {
        db.run('BEGIN TRANSACTION', (err) => {
          if (err) reject(err);
          else resolve();
        });
      });
    });

    const checkLog = (uid, dir, timestamp) => {
      return new Promise((resolve, reject) => {
        db.get('SELECT id FROM attendance_logs WHERE uid = ? AND direction = ? AND timestamp = ?', [uid, dir, timestamp], (err, row) => {
          if (err) reject(err);
          else resolve(row);
        });
      });
    };

    const insertLog = (uid, name, role, dir, status, timestamp) => {
      return new Promise((resolve, reject) => {
        db.run('INSERT INTO attendance_logs (uid, employee_name, role, direction, status, timestamp) VALUES (?, ?, ?, ?, ?, ?)',
          [uid, name, role, dir, status, timestamp],
          (err) => {
            if (err) reject(err);
            else resolve();
          }
        );
      });
    };

    // Sequentially process each log
    for (const log of logs) {
      const { uid, name, role, dir, status, timestamp } = log;
      const existing = await checkLog(uid, dir, timestamp);
      if (existing) {
        skipped++;
      } else {
        await insertLog(uid, name || 'Unknown', role || '-', dir || 'In', status || 'Accepted', timestamp);
        inserted++;
      }
    }

    // Commit transaction
    await new Promise((resolve, reject) => {
      db.run('COMMIT', (err) => {
        if (err) reject(err);
        else resolve();
      });
    });

    console.log(`💾 Sync complete! Inserted: ${inserted} | Skipped (duplicates): ${skipped}`);
    res.status(200).send({ success: true, inserted, skipped });
  } catch (error) {
    console.error('❌ Sync failed:', error.message);
    db.run('ROLLBACK');
    res.status(500).send({ error: 'Sync failed', details: error.message });
  }
});

// ==========================================
// 4. START SERVER
// ==========================================
const PORT = 3000;
app.listen(PORT, () => {
  console.log('==================================================');
  console.log(`🚀 Capstone Backend Server is running on port ${PORT}`);
  console.log(`🔗 Webhook URL: http://localhost:${PORT}/webhook/scan`);
  console.log('==================================================');
});
