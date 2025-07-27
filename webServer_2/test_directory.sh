#!/bin/bash

echo "🔍 Checking directory structure..."
echo "Current directory: $(pwd)"
echo ""

echo "📁 Contents of ./www:"
if [ -d "./www" ]; then
    ls -la ./www/
else
    echo "❌ ./www directory does not exist!"
    echo "Creating ./www directory and index.html..."
    mkdir -p ./www
    cat > ./www/index.html << EOF
<!DOCTYPE html>
<html>
<head>
    <title>Test Page</title>
</head>
<body>
    <h1>Hello from Webserv!</h1>
    <p>This is a test page served by the webserver.</p>
</body>
</html>
EOF
    echo "✅ Created ./www/index.html"
fi

echo ""
echo "📄 Contents of ./www/index.html:"
if [ -f "./www/index.html" ]; then
    cat ./www/index.html
    echo ""
    echo "📊 File size: $(wc -c < ./www/index.html) bytes"
else
    echo "❌ ./www/index.html does not exist!"
fi