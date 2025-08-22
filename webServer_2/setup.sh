#!/bin/bash

echo "🔧 Setting up webserv test environment..."

# Create necessary directories
mkdir -p www
mkdir -p www/videos
mkdir -p www/uploads
mkdir -p www/admin
mkdir -p CGI

# Create main index.html
cat > www/index.html << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Webserv Test Page</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            color: white;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.1);
        }
        .header {
            text-align: center;
            margin-bottom: 40px;
        }
        .header h1 {
            font-size: 3em;
            margin-bottom: 10px;
            background: linear-gradient(45deg, #ff6b6b, #feca57);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }
        .feature-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }
        .feature-card {
            background: rgba(255, 255, 255, 0.1);
            padding: 25px;
            border-radius: 15px;
            border: 1px solid rgba(255, 255, 255, 0.2);
            transition: transform 0.3s ease;
        }
        .feature-card:hover {
            transform: translateY(-5px);
        }
        .feature-card h3 {
            color: #feca57;
            margin-bottom: 15px;
        }
        .button {
            display: inline-block;
            padding: 12px 24px;
            background: linear-gradient(45deg, #ff6b6b, #ee5a52);
            color: white;
            text-decoration: none;
            border-radius: 25px;
            margin: 5px;
            transition: transform 0.2s ease;
            border: none;
            cursor: pointer;
        }
        .button:hover {
            transform: scale(1.05);
        }
        .upload-form {
            margin: 20px 0;
            padding: 20px;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
        }
        .form-group {
            margin: 15px 0;
        }
        .form-group input, .form-group textarea {
            width: 100%;
            padding: 10px;
            border: none;
            border-radius: 5px;
            background: rgba(255, 255, 255, 0.9);
            color: #333;
            box-sizing: border-box;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 Webserv</h1>
            <p>HTTP Server Implementation in C++98</p>
        </div>

        <div class="feature-grid">
            <div class="feature-card">
                <h3>📊 HTTP Methods</h3>
                <p>Support for GET, POST, and DELETE methods with proper status codes and error handling.</p>
            </div>
            <div class="feature-card">
                <h3>📁 Static Files</h3>
                <p>Serve static HTML, CSS, JS, and other files with automatic content-type detection.</p>
            </div>
            <div class="feature-card">
                <h3>📂 Directory Listing</h3>
                <p>Configurable directory browsing with clean, modern interface.</p>
            </div>
            <div class="feature-card">
                <h3>📤 File Upload</h3>
                <p>Handle file uploads with configurable storage locations.</p>
            </div>
        </div>

        <div style="margin: 30px 0; padding: 20px; background: rgba(255, 255, 255, 0.05); border-radius: 10px;">
            <h2>🧪 Test Different Features</h2>
            
            <h3>Navigation</h3>
            <a href="/videos" class="button">Videos Directory</a>
            <a href="/admin" class="button">Admin Area</a>
            <a href="/upload" class="button">Upload Directory</a>
            
            <div class="upload-form">
                <h3>📤 File Upload Test</h3>
                <form action="/upload" method="post" enctype="application/x-www-form-urlencoded">
                    <div class="form-group">
                        <label>Test Upload Data:</label>
                        <input type="text" name="test_data" placeholder="Enter some test data..." required>
                    </div>
                    <div class="form-group">
                        <label>Description (optional):</label>
                        <textarea name="description" rows="3" placeholder="Describe your data..."></textarea>
                    </div>
                    <button type="submit" class="button">Upload Data</button>
                </form>
            </div>

            <div style="margin: 20px 0;">
                <h3>🔗 HTTP Status Code Tests</h3>
                <p>Test various HTTP responses:</p>
                <a href="/nonexistent" class="button">404 Test</a>
                <a href="/forbidden" class="button">403 Test</a>
            </div>
        </div>

        <div style="margin: 30px 0; padding: 20px; background: rgba(255, 255, 255, 0.05); border-radius: 10px;">
            <h2>📋 Server Information</h2>
            <p><strong>Server:</strong> webserv/1.0</p>
            <p><strong>HTTP Version:</strong> HTTP/1.1</p>
            <p><strong>Features:</strong> Static files, File upload, Directory listing</p>
            <p><strong>Methods:</strong> GET, POST, DELETE</p>
        </div>
    </div>

    <script>
        // Simple JavaScript to test functionality
        console.log('Webserv test page loaded successfully!');
        
        // Add click handlers for testing
        document.addEventListener('DOMContentLoaded', function() {
            console.log('DOM loaded, webserv is working!');
        });
    </script>
</body>
</html>
EOF

# Create test files in videos directory
cat > www/videos/index.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>Videos Directory</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }
        .container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .back-link { color: #007bff; text-decoration: none; }
    </style>
</head>
<body>
    <div class="container">
        <h1>📹 Videos Directory</h1>
        <p>This is the videos section. Directory listing should be enabled here.</p>
        <p>You can upload video files to this directory and they will be listed automatically.</p>
        <p><a href="/" class="back-link">← Back to home</a></p>
    </div>
</body>
</html>
EOF

# Create sample video file
echo "This is a sample video file for testing." > www/videos/sample.txt

# Create admin directory with index
cat > www/admin/index.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>Admin Area</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #e3f2fd; }
        .container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .warning { background: #fff3cd; padding: 15px; border-radius: 5px; border-left: 4px solid #ffc107; margin: 20px 0; }
        .back-link { color: #007bff; text-decoration: none; }
    </style>
</head>
<body>
    <div class="container">
        <h1>⚙️ Admin Area</h1>
        <div class="warning">
            <strong>⚠️ Restricted Area</strong><br>
            This area supports all HTTP methods: GET, POST, DELETE, PUT
        </div>
        <p>Admin functions and CGI scripts can be placed here.</p>
        <p>CGI extension is configured for .php files in this location.</p>
        <p><a href="/" class="back-link">← Back to home</a></p>
    </div>
</body>
</html>
EOF

# Create uploads directory with index
cat > www/uploads/index.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>Upload Directory</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #e8f5e8; }
        .container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .back-link { color: #007bff; text-decoration: none; }
    </style>
</head>
<body>
    <div class="container">
        <h1>📤 Upload Directory</h1>
        <p>Files uploaded through the POST method will appear here.</p>
        <p>Directory listing is enabled to show uploaded files.</p>
        <p><a href="/" class="back-link">← Back to home</a></p>
    </div>
</body>
</html>
EOF

# Create a simple test file for deletion testing
echo "This file can be deleted using the DELETE method." > www/test-delete.txt

# Set permissions
chmod -R 755 www
chmod +x setup.sh

echo "✅ Test environment setup complete!"
echo "📁 Created directories: www, www/videos, www/uploads, www/admin"
echo "📄 Created test files and index pages"
echo "🚀 You can now run: make && ./webserv"
echo "🌐 Then open http://localhost:9090 in your browser"