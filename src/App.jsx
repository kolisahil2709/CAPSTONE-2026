import { useState, useRef, useEffect, useCallback } from 'react';
import { Calculator, Upload, FileText, Download, Zap, Building2, User, Calendar, IndianRupee, Settings, HelpCircle, X, DownloadCloud, ArrowLeft, RefreshCw, ShieldCheck, Clock, ChevronRight } from 'lucide-react';
import Papa from 'papaparse';
import { useReactToPrint } from 'react-to-print';
import { ToWords } from 'to-words';
import './App.css';

const toWords = new ToWords({
  localeCode: 'en-IN',
  converterOptions: {
    currency: true,
    ignoreDecimal: false,
    ignoreZeroCurrency: false,
    doNotAddOnly: false,
  }
});

const InputField = ({ label, val, setVal, type = "text" }) => (
  <div className="input-group">
    <label className="input-label">{label}</label>
    <input type={type} className="input-field" value={val} onChange={(e) => setVal(type === 'number' ? Number(e.target.value) : e.target.value)} />
  </div>
);

function App() {
  // Navigation State
  const [currentPage, setCurrentPage] = useState('landing'); // 'landing', 'calculator', 'settings'

  // Application State
  const [showModal, setShowModal] = useState(false);
  const payslipRef = useRef();

  // Payroll Configuration
  const [autoCalc, setAutoCalc] = useState(true);
  const [enableLop, setEnableLop] = useState(false);
  const [pfRateEmp, setPfRateEmp] = useState(12);
  const [pfRateEmplr, setPfRateEmplr] = useState(12);
  const [pfLimit, setPfLimit] = useState(15000);
  const [esiRateEmp, setEsiRateEmp] = useState(0.75);
  const [esiRateEmplr, setEsiRateEmplr] = useState(3.25);
  const [esiLimit, setEsiLimit] = useState(21000);

  // Company Info
  const [compName, setCompName] = useState('');
  const [compAddress, setCompAddress] = useState('');
  const [compContact, setCompContact] = useState('');

  // Employee Info
  const [empName, setEmpName] = useState('');
  const [empCode, setEmpCode] = useState('');
  const [empDept, setEmpDept] = useState('');
  const [empDesig, setEmpDesig] = useState('');
  const [payMonth, setPayMonth] = useState('');
  const [payslipNo, setPayslipNo] = useState('');
  const [pfNo, setPfNo] = useState('');
  const [esiNo, setEsiNo] = useState('');
  const [pan, setPan] = useState('');
  const [bankAc, setBankAc] = useState('');

  // Attendance
  const [totalDays, setTotalDays] = useState(31);
  const [workingDays, setWorkingDays] = useState(0);
  const [presentDays, setPresentDays] = useState(0);
  const [wfhDays, setWfhDays] = useState(0);
  const [absentDays, setAbsentDays] = useState(0);
  const [leaveDays, setLeaveDays] = useState(0);

  // Earnings
  const [earnBasic, setEarnBasic] = useState(0);
  const [earnDa, setEarnDa] = useState(0);
  const [earnHra, setEarnHra] = useState(0);
  const [earnConveyance, setEarnConveyance] = useState(0);
  const [earnMedical, setEarnMedical] = useState(0);
  const [earnSpecial, setEarnSpecial] = useState(0);
  const [earnOvertime, setEarnOvertime] = useState(0);
  const [earnPerformance, setEarnPerformance] = useState(0);

  // Deductions
  const [dedPf, setDedPf] = useState(0);
  const [dedEsi, setDedEsi] = useState(0);
  const [dedTax, setDedTax] = useState(0);
  const [dedLop, setDedLop] = useState(0);
  const [dedLoan, setDedLoan] = useState(0);
  const [dedProfTax, setDedProfTax] = useState(0);

  // Employer Contributions
  const [empPf, setEmpPf] = useState(0);
  const [empEps, setEmpEps] = useState(0);
  const [empEsi, setEmpEsi] = useState(0);

  const [results, setResults] = useState({
    totalEarnings: 0,
    totalDeductions: 0,
    netPay: 0,
    netPayWords: 'Zero Rupees Only'
  });

  const handlePrint = useReactToPrint({
    contentRef: payslipRef,
    documentTitle: `${empName.replace(/\s+/g, '_')}_Payslip_${payMonth.replace(/\s+/g, '_')}`,
  });

  // Timeout & Reset Logic
  const resetAppState = useCallback(() => {
    setAutoCalc(true);
    setEnableLop(false);
    setPfRateEmp(12);
    setPfRateEmplr(12);
    setPfLimit(15000);
    setEsiRateEmp(0.75);
    setEsiRateEmplr(3.25);
    setEsiLimit(21000);

    setCompName(''); setCompAddress(''); setCompContact('');
    setEmpName(''); setEmpCode(''); setEmpDept(''); setEmpDesig(''); setPayMonth(''); setPayslipNo(''); setPfNo(''); setEsiNo(''); setPan(''); setBankAc('');
    setTotalDays(31); setWorkingDays(0); setPresentDays(0); setWfhDays(0); setAbsentDays(0); setLeaveDays(0);
    setEarnBasic(0); setEarnDa(0); setEarnHra(0); setEarnConveyance(0); setEarnMedical(0); setEarnSpecial(0); setEarnOvertime(0); setEarnPerformance(0);
    setDedPf(0); setDedEsi(0); setDedTax(0); setDedLop(0); setDedLoan(0); setDedProfTax(0);
    setEmpPf(0); setEmpEps(0); setEmpEsi(0);

    setResults({
      totalEarnings: 0,
      totalDeductions: 0,
      netPay: 0,
      netPayWords: 'Zero Rupees Only'
    });
  }, []);

  useEffect(() => {
    let timeoutId;

    const resetTimeout = () => {
      clearTimeout(timeoutId);
      // 2 minutes = 120,000 ms
      timeoutId = setTimeout(() => {
        if (currentPage !== 'landing') {
          resetAppState();
          setCurrentPage('landing');
        }
      }, 120000);
    };

    if (currentPage !== 'landing') {
      resetTimeout(); // start initial timer
      const events = ['mousemove', 'keydown', 'click', 'scroll', 'touchstart'];
      events.forEach(event => window.addEventListener(event, resetTimeout));

      return () => {
        clearTimeout(timeoutId);
        events.forEach(event => window.removeEventListener(event, resetTimeout));
      };
    }
  }, [currentPage, resetAppState]);


  // Manual Calculation Engine
  const calculatePayroll = () => {
    let currentHra = earnHra;
    let currentDedPf = dedPf;
    let currentEmpEps = empEps;
    let currentEmpPf = empPf;
    let currentDedEsi = dedEsi;
    let currentEmpEsi = empEsi;
    let currentDedLop = dedLop;

    if (autoCalc) {
      currentHra = Math.round(earnBasic * 0.50);
      setEarnHra(currentHra);

      const pfBasis = Math.min(earnBasic, pfLimit);
      currentDedPf = Math.round(pfBasis * (pfRateEmp / 100));
      setDedPf(currentDedPf);
      
      currentEmpEps = Math.round(pfBasis * 0.0833);
      setEmpEps(currentEmpEps);

      currentEmpPf = Math.round(pfBasis * 0.0367);
      setEmpPf(currentEmpPf);

      const tempGross = earnBasic + currentHra + earnDa + earnConveyance + earnMedical + earnSpecial + earnOvertime + earnPerformance;
      
      if (tempGross <= esiLimit) {
        currentDedEsi = Math.ceil(tempGross * (esiRateEmp / 100));
        currentEmpEsi = Math.ceil(tempGross * (esiRateEmplr / 100));
      } else {
        currentDedEsi = 0;
        currentEmpEsi = 0;
      }
      setDedEsi(currentDedEsi);
      setEmpEsi(currentEmpEsi);

      // LOP Logic
      if (enableLop && totalDays > 0) {
        currentDedLop = Math.round((tempGross / totalDays) * absentDays);
      } else if (!enableLop) {
        currentDedLop = 0;
      }
      setDedLop(currentDedLop);
    }

    const totalE = earnBasic + earnDa + currentHra + earnConveyance + earnMedical + earnSpecial + earnOvertime + earnPerformance;
    const totalD = currentDedPf + currentDedEsi + dedTax + currentDedLop + dedLoan + dedProfTax;
    const net = Math.max(0, totalE - totalD);
    
    let words = '';
    try { words = toWords.convert(net); } catch (e) { words = 'Zero Rupees Only'; }

    setResults({
      totalEarnings: totalE,
      totalDeductions: totalD,
      netPay: net,
      netPayWords: words
    });
  };

  const handleFileUpload = (e) => {
    const file = e.target.files[0];
    if (file) {
      Papa.parse(file, {
        header: true,
        skipEmptyLines: true,
        complete: (parsedResults) => {
          if (parsedResults.data.length > 0) {
            const d = parsedResults.data[0];
            if (d.CompName) setCompName(d.CompName);
            if (d.CompAddress) setCompAddress(d.CompAddress);
            if (d.CompContact) setCompContact(d.CompContact);
            if (d.EmpName) setEmpName(d.EmpName);
            if (d.EmpCode) setEmpCode(d.EmpCode);
            if (d.EmpDept) setEmpDept(d.EmpDept);
            if (d.EmpDesig) setEmpDesig(d.EmpDesig);
            if (d.PayMonth) setPayMonth(d.PayMonth);
            if (d.PayslipNo) setPayslipNo(d.PayslipNo);
            if (d.PfNo) setPfNo(d.PfNo);
            if (d.EsiNo) setEsiNo(d.EsiNo);
            if (d.Pan) setPan(d.Pan);
            if (d.BankAc) setBankAc(d.BankAc);
            if (d.EarnBasic) setEarnBasic(Number(d.EarnBasic));
            if (d.EarnDa) setEarnDa(Number(d.EarnDa));
            if (d.EarnHra) setEarnHra(Number(d.EarnHra));
            if (d.EarnConveyance) setEarnConveyance(Number(d.EarnConveyance));
            if (d.EarnMedical) setEarnMedical(Number(d.EarnMedical));
            if (d.EarnSpecial) setEarnSpecial(Number(d.EarnSpecial));
            if (d.EarnOvertime) setEarnOvertime(Number(d.EarnOvertime));
            if (d.EarnPerformance) setEarnPerformance(Number(d.EarnPerformance));
            if (d.DedPf) setDedPf(Number(d.DedPf));
            if (d.DedEsi) setDedEsi(Number(d.DedEsi));
            if (d.DedTax) setDedTax(Number(d.DedTax));
            if (d.DedLop) setDedLop(Number(d.DedLop));
            if (d.DedLoan) setDedLoan(Number(d.DedLoan));
            if (d.DedProfTax) setDedProfTax(Number(d.DedProfTax));
            if (d.EmpPf) setEmpPf(Number(d.EmpPf));
            if (d.EmpEps) setEmpEps(Number(d.EmpEps));
            if (d.EmpEsi) setEmpEsi(Number(d.EmpEsi));
            
            alert('File uploaded successfully! Please click "SAVE and CALCULATE" to refresh.');
          }
        }
      });
    }
  };

  const handleDownloadTemplate = () => {
    const headers = ["CompName","CompAddress","CompContact","EmpName","EmpCode","EmpDept","EmpDesig","PayMonth","PayslipNo","PfNo","EsiNo","Pan","BankAc","EarnBasic","EarnDa","EarnHra","EarnConveyance","EarnMedical","EarnSpecial","EarnOvertime","EarnPerformance","DedPf","DedEsi","DedTax","DedLop","DedLoan","DedProfTax","EmpPf","EmpEps","EmpEsi"];
    const emptyRow = new Array(headers.length).fill('');
    const csvContent = headers.join(',') + '\n' + emptyRow.join(',');
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement("a");
    const url = URL.createObjectURL(blob);
    link.setAttribute("href", url);
    link.setAttribute("download", "payroll_template.csv");
    link.style.visibility = 'hidden';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  // --- Views ---

  if (currentPage === 'landing') {
    return (
      <div className="landing-container">
        {/* Animated Background Overlay */}
        <div className="hero-background"></div>
        
        <header className="landing-header animate-fade-in">
          <div className="logo-container" style={{ margin: 0 }}>
            <Zap className="logo-icon bounce" size={32} color="var(--primary-accent)" />
            <h1 className="gradient-text" style={{ fontSize: '2rem', letterSpacing: '1px' }}>ULTRAPAY</h1>
          </div>
        </header>

        <main className="landing-main">
          <div className="hero-content">
            <div className="badge animate-fade-in">v2.0 • Secure & Offline</div>
            <h1 className="hero-title animate-fade-in" style={{ animationDelay: '0.1s' }}>
              Automated Payroll, <br /> Built for Modern Teams.
            </h1>
            <p className="hero-subtitle animate-fade-in" style={{ animationDelay: '0.2s' }}>
              Standard Indian Tabular Payslip Generator. Accurate, compliant, and effortless. Calculate PF, ESI, and Taxes in seconds without any data leaving your browser.
            </p>
            
            <button className="btn btn-primary hero-btn animate-fade-in" style={{ animationDelay: '0.3s' }} onClick={() => setCurrentPage('calculator')}>
              Start Calculating <ChevronRight size={20} />
            </button>
          </div>

          <div className="features-grid animate-fade-in" style={{ animationDelay: '0.4s' }}>
            <div className="feature-card glass-panel">
              <div className="feature-icon"><ShieldCheck size={28} /></div>
              <h3>Statutory Compliant</h3>
              <p>Automatically enforces Indian limits and calculation rules for PF, EPS, and ESI components.</p>
            </div>
            
            <div className="feature-card glass-panel">
              <div className="feature-icon"><Clock size={28} /></div>
              <h3>Dynamic LOP Engine</h3>
              <p>Advanced attendance module to accurately auto-calculate deductions for absent days based on gross pay.</p>
            </div>
            
            <div className="feature-card glass-panel">
              <div className="feature-icon"><DownloadCloud size={28} /></div>
              <h3>Instant Export</h3>
              <p>Generate pixel-perfect, ready-to-share PDF payslips exactly formatted for standard HR policies.</p>
            </div>
          </div>
        </main>
      </div>
    );
  }

  if (currentPage === 'settings') {
    return (
      <div className="app-container animate-fade-in" style={{ padding: '40px 20px', maxWidth: '800px', margin: '0 auto' }}>
        <button className="btn btn-secondary action-btn" style={{ marginBottom: '20px' }} onClick={() => setCurrentPage('calculator')}>
          <ArrowLeft size={16} /> Back
        </button>
        <div className="glass-panel">
          <h2 style={{color: 'var(--primary-accent)', marginBottom: '1.5rem'}}><Settings size={22} className="icon-heading" /> Payroll Configuration</h2>
          
          <div className="settings-toggles" style={{display: 'flex', flexDirection: 'column', gap: '1rem', marginBottom: '2rem'}}>
            <label className="toggle-switch">
              <input type="checkbox" checked={autoCalc} onChange={(e) => setAutoCalc(e.target.checked)} />
              <span className="toggle-slider"></span>
              <span className="toggle-label" style={{marginLeft: '10px'}}>Auto-Calculate Dynamic Rules (PF, ESI, HRA)</span>
            </label>

            <label className="toggle-switch">
              <input type="checkbox" checked={enableLop} onChange={(e) => setEnableLop(e.target.checked)} />
              <span className="toggle-slider"></span>
              <span className="toggle-label" style={{marginLeft: '10px'}}>Enable Loss of Pay (LOP) & Attendance</span>
            </label>
          </div>

          <h3 style={{fontSize: '1rem', marginBottom: '1rem', color: 'var(--text-secondary)'}}>Rates & Limits</h3>
          <div className="input-grid" style={{gridTemplateColumns: 'repeat(2, 1fr)'}}>
            <InputField label="PF Rate (Employee %)" val={pfRateEmp} setVal={setPfRateEmp} type="number" />
            <InputField label="PF Rate (Employer %)" val={pfRateEmplr} setVal={setPfRateEmplr} type="number" />
            <InputField label="PF Limit (Rs.)" val={pfLimit} setVal={setPfLimit} type="number" />
            <div style={{gridColumn: '1 / -1'}}><hr className="divider" style={{margin: '0.5rem 0'}}/></div>
            <InputField label="ESI Rate (Employee %)" val={esiRateEmp} setVal={setEsiRateEmp} type="number" />
            <InputField label="ESI Rate (Employer %)" val={esiRateEmplr} setVal={setEsiRateEmplr} type="number" />
            <InputField label="ESI Limit (Rs.)" val={esiLimit} setVal={setEsiLimit} type="number" />
          </div>

          <button className="btn btn-primary" style={{marginTop: '2rem', width: '100%'}} onClick={() => setCurrentPage('calculator')}>
            Save Settings
          </button>
        </div>
      </div>
    );
  }

  // --- Calculator View (Page 2) ---
  return (
    <div className="app-container">
      {/* Top Action Bar */}
      <div className="top-actions-bar animate-fade-in">
        <div className="file-input-wrapper">
          <input type="file" accept=".csv" id="csvInput" onChange={handleFileUpload} className="file-input" />
          <label htmlFor="csvInput" className="btn btn-secondary action-btn">
            <Upload size={16} /> Upload CSV
          </label>
        </div>
        <button onClick={handleDownloadTemplate} className="btn btn-secondary action-btn">
          <DownloadCloud size={16} /> Download Template
        </button>
        <button onClick={() => setCurrentPage('settings')} className="btn btn-primary action-btn">
          <Settings size={16} /> Settings
        </button>
      </div>

      <header className="app-header animate-fade-in" style={{marginTop: '2rem'}}>
        <div className="logo-container" onClick={() => setCurrentPage('landing')} style={{cursor: 'pointer'}}>
          <Zap className="logo-icon" size={32} color="var(--primary-accent)" />
          <h1 className="gradient-text">ULTRAPAY</h1>
        </div>
        <p className="subtitle">Standard Indian Tabular Payslip Generator</p>
      </header>

      <main className="main-content">
        <div className="inputs-panel animate-fade-in" style={{ animationDelay: '0.1s' }}>
          
          <div className="glass-panel" style={{marginBottom: '2rem'}}>
            <div className="input-section">
              <h2><Building2 size={20} className="icon-heading" /> Company Details</h2>
              <div className="input-grid">
                <InputField label="Company Name" val={compName} setVal={setCompName} />
                <InputField label="Contact Info" val={compContact} setVal={setCompContact} />
                <div className="input-group full-width">
                  <label className="input-label">Address</label>
                  <textarea className="input-field" value={compAddress} onChange={(e)=>setCompAddress(e.target.value)} rows="2"></textarea>
                </div>
              </div>
            </div>

            <hr className="divider" />

            <div className="input-section">
              <h2><User size={20} className="icon-heading" /> Employee Details</h2>
              <div className="input-grid">
                <InputField label="Name" val={empName} setVal={setEmpName} />
                <InputField label="Emp Code" val={empCode} setVal={setEmpCode} />
                <InputField label="Department" val={empDept} setVal={setEmpDept} />
                <InputField label="Designation" val={empDesig} setVal={setEmpDesig} />
                <InputField label="Month" val={payMonth} setVal={setPayMonth} />
                <InputField label="Payslip No" val={payslipNo} setVal={setPayslipNo} />
                <InputField label="PF No" val={pfNo} setVal={setPfNo} />
                <InputField label="ESI No" val={esiNo} setVal={setEsiNo} />
                <InputField label="PAN" val={pan} setVal={setPan} />
                <InputField label="Bank A/c" val={bankAc} setVal={setBankAc} />
              </div>
            </div>

            <hr className="divider" />

            <div className="input-section">
              <h2><IndianRupee size={20} className="icon-heading" /> Earnings</h2>
              <div className="input-grid">
                <InputField label="Basic" val={earnBasic} setVal={setEarnBasic} type="number" />
                <InputField label="DA" val={earnDa} setVal={setEarnDa} type="number" />
                <InputField label="HRA" val={earnHra} setVal={setEarnHra} type="number" />
                <InputField label="Conveyance" val={earnConveyance} setVal={setEarnConveyance} type="number" />
                <InputField label="Medical" val={earnMedical} setVal={setEarnMedical} type="number" />
                <InputField label="Special" val={earnSpecial} setVal={setEarnSpecial} type="number" />
                <InputField label="Overtime" val={earnOvertime} setVal={setEarnOvertime} type="number" />
                <InputField label="Performance" val={earnPerformance} setVal={setEarnPerformance} type="number" />
              </div>
            </div>

            <hr className="divider" />

            <div className="input-section">
              <h2><Calculator size={20} className="icon-heading" /> Deductions</h2>
              <div className="input-grid">
                <InputField label="PF (Employee)" val={dedPf} setVal={setDedPf} type="number" />
                <InputField label="ESI (Employee)" val={dedEsi} setVal={setDedEsi} type="number" />
                <InputField label="Income Tax" val={dedTax} setVal={setDedTax} type="number" />
                <InputField label="LOP Deduction" val={dedLop} setVal={setDedLop} type="number" />
                <InputField label="Loan Recovery" val={dedLoan} setVal={setDedLoan} type="number" />
                <InputField label="Professional Tax" val={dedProfTax} setVal={setDedProfTax} type="number" />
              </div>
            </div>

            <hr className="divider" />

            <div className="input-section" style={{marginBottom: 0}}>
              <h2><Building2 size={20} className="icon-heading" /> Employer Contributions</h2>
              <div className="input-grid">
                <InputField label="Employer PF" val={empPf} setVal={setEmpPf} type="number" />
                <InputField label="EPS" val={empEps} setVal={setEmpEps} type="number" />
                <InputField label="Employer ESI" val={empEsi} setVal={setEmpEsi} type="number" />
              </div>
            </div>
          </div>

          {enableLop && (
            <div className="glass-panel" style={{marginBottom: '2rem'}}>
              <div className="input-section" style={{marginBottom: 0}}>
                <h2><Calendar size={20} className="icon-heading" /> Attendance (LOP Enabled)</h2>
                <p className="upload-help" style={{marginBottom: '1rem'}}>
                  LOP Deduction is auto-calculated as (Gross / Total Days) * Absent Days.
                </p>
                <div className="input-grid">
                  <InputField label="Total Days" val={totalDays} setVal={setTotalDays} type="number" />
                  <InputField label="Working" val={workingDays} setVal={setWorkingDays} type="number" />
                  <InputField label="Present" val={presentDays} setVal={setPresentDays} type="number" />
                  <InputField label="WFH" val={wfhDays} setVal={setWfhDays} type="number" />
                  <InputField label="Absent (For LOP)" val={absentDays} setVal={setAbsentDays} type="number" />
                  <InputField label="Leave" val={leaveDays} setVal={setLeaveDays} type="number" />
                </div>
              </div>
            </div>
          )}

          {/* New Calculate Button Area */}
          <div className="glass-panel" style={{ textAlign: 'center', marginBottom: '2rem' }}>
            <button className="btn btn-primary" style={{ padding: '15px 40px', fontSize: '1.1rem', width: '100%', display: 'flex', justifyContent: 'center', alignItems: 'center' }} onClick={calculatePayroll}>
              <RefreshCw size={20} style={{ marginRight: '10px' }} /> SAVE and CALCULATE
            </button>
          </div>

        </div>

        {/* Payslip Preview Section */}
        <div className="glass-panel preview-panel animate-fade-in" style={{ animationDelay: '0.2s' }}>
          <div className="results-header">
            <h2>Payslip Preview</h2>
            <div style={{display: 'flex', gap: '1rem'}}>
              <button onClick={() => setShowModal(true)} className="btn btn-secondary">
                <HelpCircle size={18} /> Explain Calculation
              </button>
              <button onClick={() => handlePrint()} className="btn btn-primary">
                <Download size={18} /> Export PDF
              </button>
            </div>
          </div>
          
          <div className="payslip-paper-wrapper">
            <div className="payslip-document" ref={payslipRef}>
              
              {/* Header */}
              <div className="ps-header">
                <h1 className="ps-comp-name">{compName || 'COMPANY NAME'}</h1>
                <p className="ps-comp-address">
                  {compAddress ? compAddress.split('\n').map((line, i) => <span key={i}>{line}<br/></span>) : 'Company Address Line'}
                </p>
                <p className="ps-comp-contact">{compContact || 'Contact Info'}</p>
              </div>

              {/* Employee Details Table */}
              <table className="ps-table ps-emp-table">
                <tbody>
                  <tr>
                    <td className="ps-label">Name</td>
                    <td className="ps-val">{empName || '-'}</td>
                    <td className="ps-label">Emp Code</td>
                    <td className="ps-val">{empCode || '-'}</td>
                  </tr>
                  <tr>
                    <td className="ps-label">Department</td>
                    <td className="ps-val">{empDept || '-'}</td>
                    <td className="ps-label">Designation</td>
                    <td className="ps-val">{empDesig || '-'}</td>
                  </tr>
                  <tr>
                    <td className="ps-label">Month</td>
                    <td className="ps-val">{payMonth || '-'}</td>
                    <td className="ps-label">Payslip No</td>
                    <td className="ps-val">{payslipNo || '-'}</td>
                  </tr>
                  <tr>
                    <td className="ps-label">PF No</td>
                    <td className="ps-val">{pfNo || '-'}</td>
                    <td className="ps-label">ESI No</td>
                    <td className="ps-val">{esiNo || '-'}</td>
                  </tr>
                  <tr>
                    <td className="ps-label">PAN</td>
                    <td className="ps-val">{pan || '-'}</td>
                    <td className="ps-label">Bank A/c</td>
                    <td className="ps-val">{bankAc || '-'}</td>
                  </tr>
                </tbody>
              </table>

              {/* Attendance Table (Conditional in PDF) */}
              {enableLop && (
                <table className="ps-table ps-att-table">
                  <thead>
                    <tr>
                      <th>Days</th>
                      <th>Working</th>
                      <th>Present</th>
                      <th>WFH</th>
                      <th>Absent(LOP)</th>
                      <th>Leave</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr>
                      <td>{totalDays}</td>
                      <td>{workingDays}</td>
                      <td>{presentDays}</td>
                      <td>{wfhDays}</td>
                      <td>{absentDays}</td>
                      <td>{leaveDays}</td>
                    </tr>
                  </tbody>
                </table>
              )}

              {/* Salary Structure Table */}
              <div className="ps-salary-wrapper" style={{ marginTop: enableLop ? '0' : '10px' }}>
                <table className="ps-table ps-earn-table">
                  <thead>
                    <tr className="ps-shaded-head">
                      <th colSpan="2">Earnings</th>
                    </tr>
                    <tr>
                      <th className="text-left">Description</th>
                      <th className="text-right">Amount (Rs.)</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr><td>Basic</td><td className="text-right">{earnBasic} Rs.</td></tr>
                    <tr><td>DA</td><td className="text-right">{earnDa} Rs.</td></tr>
                    <tr><td>HRA</td><td className="text-right">{earnHra} Rs.</td></tr>
                    <tr><td>Conveyance Allowance</td><td className="text-right">{earnConveyance} Rs.</td></tr>
                    <tr><td>Medical Allowance</td><td className="text-right">{earnMedical} Rs.</td></tr>
                    <tr><td>Special Allowance</td><td className="text-right">{earnSpecial} Rs.</td></tr>
                    <tr><td>Overtime Pay</td><td className="text-right">{earnOvertime} Rs.</td></tr>
                    <tr><td>Performance Allowance</td><td className="text-right">{earnPerformance} Rs.</td></tr>
                    <tr className="ps-total-row">
                      <td>Total Earnings</td>
                      <td className="text-right">{results.totalEarnings} Rs.</td>
                    </tr>
                  </tbody>
                </table>

                <table className="ps-table ps-ded-table">
                  <thead>
                    <tr className="ps-shaded-head">
                      <th colSpan="2">Deductions</th>
                    </tr>
                    <tr>
                      <th className="text-left">Description</th>
                      <th className="text-right">Amount (Rs.)</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr><td>PF (Employee)</td><td className="text-right">{dedPf} Rs.</td></tr>
                    <tr><td>ESI (Employee)</td><td className="text-right">{dedEsi} Rs.</td></tr>
                    <tr><td>Income Tax</td><td className="text-right">{dedTax} Rs.</td></tr>
                    <tr><td>LOP Deduction</td><td className="text-right">{dedLop} Rs.</td></tr>
                    <tr><td>Loan Recovery</td><td className="text-right">{dedLoan} Rs.</td></tr>
                    <tr><td>Professional Tax</td><td className="text-right">{dedProfTax} Rs.</td></tr>
                    <tr><td>&nbsp;</td><td>&nbsp;</td></tr>
                    <tr><td>&nbsp;</td><td>&nbsp;</td></tr>
                    <tr className="ps-total-row">
                      <td>Total Deductions</td>
                      <td className="text-right">{results.totalDeductions} Rs.</td>
                    </tr>
                  </tbody>
                </table>
              </div>

              {/* Net Pay */}
              <table className="ps-table ps-net-table">
                <tbody>
                  <tr className="ps-net-row">
                    <td className="text-center" style={{width: '70%'}}><strong>NET PAY</strong></td>
                    <td className="text-right" style={{width: '30%'}}><strong>{results.netPay} Rs.</strong></td>
                  </tr>
                  <tr>
                    <td colSpan="2" className="text-center ps-words">
                      Amount in words: {results.netPayWords}
                    </td>
                  </tr>
                </tbody>
              </table>

              {/* Footer */}
              <div className="ps-footer">
                <p className="ps-computer-gen">This is a computer created slip.</p>
                <div className="ps-sign">
                  <p>Authorized Signatory</p>
                </div>
              </div>

            </div>
          </div>

        </div>
      </main>

      {/* Explanation Modal */}
      {showModal && (
        <div className="modal-overlay animate-fade-in">
          <div className="modal-content glass-panel">
            <button className="close-btn" onClick={() => setShowModal(false)}><X size={20} /></button>
            <h2 style={{color: 'var(--primary-accent)', marginBottom: '1.5rem'}}>How is the payslip calculated?</h2>
            
            <div className="explanation-steps">
              <div className="step">
                <h3>1. Gross Earnings ({results.totalEarnings} Rs.)</h3>
                <p>This is the sum of all allowances (Basic + DA + HRA + Conveyance + etc.).</p>
                {autoCalc && <p className="calc-note">➜ <strong>HRA Auto-Calc</strong>: Set to 50% of Basic Pay ({earnBasic} * 0.50 = {earnHra} Rs.)</p>}
              </div>

              {enableLop && (
                <div className="step" style={{borderColor: '#ff6b6b'}}>
                  <h3 style={{color: '#ff6b6b'}}>Loss of Pay ({dedLop} Rs.)</h3>
                  <p>Calculated based on Gross Pay and Absent Days.</p>
                  <p className="calc-note">➜ (Gross {results.totalEarnings} ÷ {totalDays} Days) * {absentDays} Absent = {dedLop} Rs.</p>
                </div>
              )}

              <div className="step">
                <h3>2. Statutory Deductions ({dedPf + dedEsi} Rs.)</h3>
                <p><strong>Employee PF ({pfRateEmp}%):</strong> Calculated on Basic Pay (capped at {pfLimit} Rs.).</p>
                <p className="calc-note">➜ min({earnBasic}, {pfLimit}) * {pfRateEmp}% = {dedPf} Rs.</p>
                
                <p style={{marginTop: '10px'}}><strong>Employee ESI ({esiRateEmp}%):</strong> Applicable only if Gross is ≤ {esiLimit} Rs.</p>
                {results.totalEarnings <= esiLimit ? (
                  <p className="calc-note">➜ Gross ({results.totalEarnings}) * {esiRateEmp}% = {dedEsi} Rs.</p>
                ) : (
                  <p className="calc-note" style={{color: '#ff6b6b'}}>➜ Gross ({results.totalEarnings}) exceeds {esiLimit} limit. ESI is 0.</p>
                )}
              </div>

              <div className="step">
                <h3>3. Other Deductions ({dedTax + dedLoan + dedProfTax} Rs.)</h3>
                <p>Includes Income Tax, Loan Recovery, and Professional Tax.</p>
              </div>

              <div className="step highlight-step">
                <h3>4. Net Pay ({results.netPay} Rs.)</h3>
                <p><strong>Total Earnings ({results.totalEarnings}) - Total Deductions ({results.totalDeductions}) = {results.netPay} Rs.</strong></p>
              </div>

              <div className="step employer-step">
                <h3>5. Employer Contributions (Hidden on Payslip)</h3>
                <p>These are paid by the company and do not reduce your Net Pay.</p>
                <ul>
                  <li><strong>EPF ({pfRateEmplr}% split):</strong> {empPf} Rs. (3.67%)</li>
                  <li><strong>EPS:</strong> {empEps} Rs. (8.33%)</li>
                  <li><strong>ESI ({esiRateEmplr}%):</strong> {empEsi} Rs.</li>
                </ul>
              </div>
            </div>
          </div>
        </div>
      )}

    </div>
  );
}

export default App;
