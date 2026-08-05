document.addEventListener('DOMContentLoaded', () => {
    // Initialize CodeMirror
    const editorTextarea = document.getElementById('code-editor');
    const editor = CodeMirror.fromTextArea(editorTextarea, {
        mode: 'javascript', // Closest to Nuva syntax out-of-the-box
        theme: 'material-ocean',
        lineNumbers: true,
        matchBrackets: true,
        autoCloseBrackets: true,
        indentUnit: 4,
        tabSize: 4
    });

    const runBtn = document.getElementById('run-btn');
    const clearBtn = document.getElementById('clear-btn');
    const outputElem = document.getElementById('output');
    const exampleSelect = document.getElementById('example-select');

    if (exampleSelect) {
        exampleSelect.addEventListener('change', async (e) => {
            const filename = e.target.value;
            if (!filename) return;
            try {
                const response = await fetch('/examples/' + filename);
                if (response.ok) {
                    const text = await response.text();
                    editor.setValue(text);
                    // Update header if needed, but not strictly required
                } else {
                    console.error('Failed to load example:', response.status);
                }
            } catch (err) {
                console.error('Error fetching example:', err);
            }
            // Reset select so the same file can be selected again
            e.target.selectedIndex = 0;
        });
    }

    // Run code logic
    runBtn.addEventListener('click', async () => {
        const code = editor.getValue();
        if (!code.trim()) return;

        // UI State
        runBtn.classList.add('loading');
        runBtn.innerHTML = `
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="spin">
                <circle cx="12" cy="12" r="10"></circle>
                <path d="M12 2a10 10 0 0 1 10 10"></path>
            </svg> Running...
        `;
        
        outputElem.innerHTML = '<span class="output-meta">Executing...</span>\n';

        try {
            const response = await fetch('/api/run', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ code })
            });

            const data = await response.json();

            outputElem.innerHTML = ''; // Clear meta text
            
            if (data.output) {
                outputElem.innerHTML += escapeHtml(data.output);
            }
            if (data.error) {
                outputElem.innerHTML += `<span class="output-error">${escapeHtml(data.error)}</span>`;
            }
            if (!data.output && !data.error) {
                outputElem.innerHTML = '<span class="output-meta">(No output)</span>';
            }

        } catch (err) {
            outputElem.innerHTML = `<span class="output-error">Network Error: Could not connect to backend server.</span>`;
        } finally {
            // Restore UI State
            runBtn.classList.remove('loading');
            runBtn.innerHTML = `
                <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <polygon points="5 3 19 12 5 21 5 3"></polygon>
                </svg>
                Run Code
            `;
        }
    });

    clearBtn.addEventListener('click', () => {
        outputElem.innerHTML = '';
    });

    // Helper to escape HTML to prevent XSS in the terminal
    function escapeHtml(unsafe) {
        return unsafe
             .replace(/&/g, "&amp;")
             .replace(/</g, "&lt;")
             .replace(/>/g, "&gt;")
             .replace(/"/g, "&quot;")
             .replace(/'/g, "&#039;");
    }
});
