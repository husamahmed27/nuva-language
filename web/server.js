const express = require('express');
const cors = require('cors');
const { execFile } = require('child_process');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

const app = express();
const port = 3000;

app.use(cors());
app.use(express.json());
app.use(express.static('public'));
app.use('/examples', express.static(path.join(__dirname, '..', 'examples')));

// Ensure tmp directory exists
const tmpDir = path.join(__dirname, 'tmp');
if (!fs.existsSync(tmpDir)) {
    fs.mkdirSync(tmpDir);
}

app.post('/api/run', (req, res) => {
    const { code } = req.body;
    if (!code) {
        return res.status(400).json({ error: 'No code provided' });
    }

    // Generate a unique filename
    const filename = crypto.randomBytes(16).toString('hex') + '.nv';
    const filePath = path.join(tmpDir, filename);

    // Write code to file
    fs.writeFileSync(filePath, code);

    // Execute nuva compiler from parent directory
    const nuvaExe = path.join(__dirname, '..', 'nuva.exe');
    
    execFile(nuvaExe, [filePath], (error, stdout, stderr) => {
        // Clean up the temp file
        try {
            fs.unlinkSync(filePath);
        } catch (e) {
            console.error('Failed to delete temp file:', e);
        }

        // We want to return both stdout and stderr, regardless of exit code
        res.json({
            output: stdout || '',
            error: stderr || (error ? error.message : '')
        });
    });
});

// Error handling middleware to catch unhandled exceptions
app.use((err, req, res, next) => {
    console.error('Unhandled Error:', err);
    res.status(500).json({
        error: 'Internal Server Error',
        message: err.message || 'Something went wrong on the server'
    });
});

app.listen(port, () => {
    console.log(`Nuva web playground listening at http://localhost:${port}`);
});
