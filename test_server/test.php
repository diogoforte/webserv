<head>
	<title>Webserver</title>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="styles.css">
</head>
<html>
    <body>
        <h1>
            <?php echo "Hello, World!<br>"; ?>
        </h1>
        <p>
            <?php echo "The current date and time is: " . date('d-m-Y H:i:s'); ?>
        </p>
        <p>
            <?php echo "The php version is: " . phpversion(); ?>
    </body>
</html> 