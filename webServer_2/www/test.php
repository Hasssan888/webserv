<?php
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html>
<html>
<head>
    <title>CGI PHP Test</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            max-width: 800px; 
            margin: 0 auto; 
            padding: 20px;
            background: linear-gradient(135deg, #74b9ff, #0984e3);
            color: white;
            min-height: 100vh;
        }
        .container {
            background: rgba(255, 255, 255, 0.1);
            padding: 30px;
            border-radius: 15px;
            backdrop-filter: blur(10px);
        }
        .info-box {
            background: rgba(255, 255, 255, 0.1);
            padding: 15px;
            margin: 10px 0;
            border-radius: 8px;
            border-left: 4px solid #00cec9;
        }
        table { 
            width: 100%; 
            border-collapse: collapse; 
            margin: 15px 0;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 8px;
            overflow: hidden;
        }
        th, td { 
            padding: 12px; 
            text-align: left; 
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }
        th { 
            background: rgba(0, 0, 0, 0.2); 
            font-weight: bold;
        }
        .success { color: #00b894; }
        .highlight { 
            background: rgba(255, 107, 107, 0.2); 
            padding: 2px 6px; 
            border-radius: 4px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🐘 CGI PHP Test - <span class="success">SUCCESS!</span></h1>
        <p>If you can see this page, CGI is working correctly!</p>
        
        <div class="info-box">
            <h2>📊 Server Information</h2>
            <p><strong>Server Time:</strong> <?php echo date('Y-m-d H:i:s T'); ?></p>
            <p><strong>PHP Version:</strong> <?php echo phpversion(); ?></p>
            <p><strong>Server Software:</strong> <?php echo $_SERVER['SERVER_SOFTWARE'] ?? 'webserv/1.0'; ?></p>
        </div>

        <div class="info-box">
            <h2>🔍 Request Information</h2>
            <p><strong>Request Method:</strong> <span class="highlight"><?php echo $_SERVER['REQUEST_METHOD'] ?? 'Unknown'; ?></span></p>
            <p><strong>Request URI:</strong> <span class="highlight"><?php echo $_SERVER['REQUEST_URI'] ?? 'Unknown'; ?></span></p>
            <p><strong>Query String:</strong> <?php echo $_SERVER['QUERY_STRING'] ?? 'None'; ?></p>
            <p><strong>Content Type:</strong> <?php echo $_SERVER['CONTENT_TYPE'] ?? 'Not set'; ?></p>
            <p><strong>Content Length:</strong> <?php echo $_SERVER['CONTENT_LENGTH'] ?? '0'; ?> bytes</p>
        </div>

        <div class="info-box">
            <h2>🌍 Environment Variables</h2>
            <table>
                <tr><th>Variable</th><th>Value</th></tr>
                <?php
                $cgi_vars = [
                    'REQUEST_METHOD', 'REQUEST_URI', 'QUERY_STRING', 'CONTENT_TYPE', 
                    'CONTENT_LENGTH', 'SERVER_SOFTWARE', 'SERVER_NAME', 'SERVER_PORT',
                    'SCRIPT_NAME', 'SCRIPT_FILENAME', 'GATEWAY_INTERFACE', 'SERVER_PROTOCOL',
                    'HTTP_HOST', 'HTTP_USER_AGENT', 'HTTP_ACCEPT', 'HTTP_CONNECTION'
                ];
                
                foreach ($cgi_vars as $var) {
                    $value = $_SERVER[$var] ?? 'Not set';
                    echo "<tr><td><strong>$var</strong></td><td>" . htmlspecialchars($value) . "</td></tr>";
                }
                ?>
            </table>
        </div>

        <?php if ($_SERVER['REQUEST_METHOD'] === 'POST'): ?>
        <div class="info-box">
            <h2>📝 POST Data</h2>
            <?php
            $input = file_get_contents('php://input');
            if (!empty($input)) {
                echo "<p><strong>Raw POST data:</strong></p>";
                echo "<pre style='background: rgba(0,0,0,0.2); padding: 15px; border-radius: 5px; overflow-x: auto;'>";
                echo htmlspecialchars($input);
                echo "</pre>";
            } else {
                echo "<p>No POST data received.</p>";
            }
            
            if (!empty($_POST)) {
                echo "<p><strong>Parsed POST variables:</strong></p>";
                echo "<pre style='background: rgba(0,0,0,0.2); padding: 15px; border-radius: 5px;'>";
                print_r($_POST);
                echo "</pre>";
            }
            ?>
        </div>
        <?php endif; ?>

        <div class="info-box">
            <h2>🧪 Test Forms</h2>
            
            <h3>GET Test</h3>
            <form method="GET" style="margin: 10px 0;">
                <input type="text" name="test_param" placeholder="Enter test parameter" 
                       style="padding: 8px; margin: 5px; border: none; border-radius: 4px;">
                <input type="submit" value="Test GET" 
                       style="padding: 8px 15px; background: #00b894; color: white; border: none; border-radius: 4px; cursor: pointer;">
            </form>

            <h3>POST Test</h3>
            <form method="POST" style="margin: 10px 0;">
                <input type="text" name="post_data" placeholder="Enter POST data" 
                       style="padding: 8px; margin: 5px; border: none; border-radius: 4px;">
                <input type="submit" value="Test POST" 
                       style="padding: 8px 15px; background: #74b9ff; color: white; border: none; border-radius: 4px; cursor: pointer;">
            </form>
        </div>

        <?php if (!empty($_GET)): ?>
        <div class="info-box">
            <h2>📥 GET Parameters</h2>
            <table>
                <tr><th>Parameter</th><th>Value</th></tr>
                <?php foreach ($_GET as $key => $value): ?>
                <tr>
                    <td><strong><?php echo htmlspecialchars($key); ?></strong></td>
                    <td><?php echo htmlspecialchars($value); ?></td>
                </tr>
                <?php endforeach; ?>
            </table>
        </div>
        <?php endif; ?>

        <div class="info-box">
            <h2>✅ CGI Functionality Test</h2>
            <p>✓ PHP script execution: <span class="success">Working</span></p>
            <p>✓ Environment variables: <span class="success">Available</span></p>
            <p>✓ HTTP request parsing: <span class="success">Functional</span></p>
            <p>✓ Content generation: <span class="success">Dynamic</span></p>
            <p>✓ Server integration: <span class="success">Complete</span></p>
        </div>

        <p style="text-align: center; margin-top: 30px; opacity: 0.8;">
            <a href="/" style="color: #74b9ff; text-decoration: none;">← Back to Main Page</a>
        </p>
    </div>
</body>
</html>