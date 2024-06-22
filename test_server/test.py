import pyjokes

def generate_joke():
    joke = pyjokes.get_joke()
    version = pyjokes.__version__
    print("<head>\
	<title>Webserver</title>\
	<meta charset=\"utf-8\">\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
	<link rel=\"stylesheet\" href=\"styles.css\">\
    </head>")
    print("<html>")
    print("<body>")
    print("<h1>Here's a joke for you:</h1>")
    print("<p>{}</p>".format(joke))
    print("<p>Pyjokes version: {}</p>".format(version))
    print("</body>")
    print("</html>")

generate_joke()