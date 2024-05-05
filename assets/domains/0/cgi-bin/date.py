
#! /usr/bin/python3

import cgi
import os
import sys
import datetime

form = cgi.FieldStorage()
date =	datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S") 
page = "HTTP/1.1 200 OK\r\n"
page += "Date: " + date + "\r\n"
page += "Server: Awesome SAD Server/1.0\r\n"
page += "Content-Type: text/html\r\n"
page += "Connection: close"
page += "\r\n"
page += "\r\n"
page += '''
<!DOCTYPE html>
<html>
<head>
<title>Webserv | Server Tests</title>
<meta charset="utf-8">
<link rel="stylesheet" href="/styles/reset.css" type="text/css">
<link rel="stylesheet" href="/styles/styl.css" type="text/css">
<link href='http://fonts.googleapis.com/css?family=Source+Sans+Pro:200,400,600,700&subset=latin,latin-ext' rel='stylesheet' type='text/css'>
<script>
    function uploadFile(event) {
        event.preventDefault();
        const input = document.getElementById("fileInput");
        const file = input.files[0];

        if (file) {
            const reader = new FileReader();

            reader.onload = (e) => {
                const fileContent = e.target.result;
                const filenName = file.name;
                const uploadUrl = '/uploads/${fileName}';
                fetch(uploadUrl, {
                    method: "PUT",
                    headers: {
                        "Content-Type": "application/octet-stream",
                        "Content-Disposition": `attachment; filename="${file.name}"`,
                    },
                    body: fileContent,
                })
                .then((response) => {
                    if (response.ok) {
                        alert("File uploaded successfully!");
						document.getElementById("filename").placeholder = file.name;
                    } else {
                        alert("Failed to upload file.");
                    }
                })
                .catch((error) => {
                    console.error("Error uploading file:", error);
                });
            };

            reader.readAsArrayBuffer(file);
        } else {
            alert("Please select a file to upload.");
        }
    }
</script>
<script>
	function sendDeleteRequest(event) {
		event.preventDefault(); // Prevent default form submission
		const filename = document.getElementById("filename").value.trim(); // Trim spaces

		if (!filename) { // Ensure filename is not empty
			alert("Please enter a valid filename.");
			return false;
		}

		console.log("Deleting file:", filename);

		fetch(`/uploads/${encodeURIComponent(filename)}`, {
			method: 'DELETE'
		})
		.then(response => {
			if (response.ok) {
				alert('File deleted successfully!'); // Success message
			} else {
				alert('Error deleting file. Please try again.'); // Failure message
			}
		})
		.catch(error => {
			console.error('Error deleting file:', error); // Error handling
		});

		return false; // Prevent default form submission
	}
</script>
</head>
<body>
<div id="header">
  <div id="header_in">
    <h1><a href="/index.html"><b>WEBSERV</b></a></h1>
    <div id="menu">
      <ul>
        <li><a href="/index.html">Home</a></li>
        <li><a href="/about.html">About</a></li>
        <li><a href="/tests.html" class="active">Server Tests</a></li>
        <li><a href="/cgi-bin/session.py">Session</a></li>
      </ul>
    </div>
  </div>
</div>
<div id="main_part_inner">
	<div id="main_part_inner_in">
	  <h2>Server Tests</h2>
  </div>
</div>
<div>
	<h3>Click the button to get the current date:</h3>
	<form method="post" action="/cgi-bin/date.py" class="formit">
		<input type="submit" class="button_submit" value="PYTHON SCRIPT: DATE">
		<input type="text" value="'''	
page += date
page +='''">
	</form>
</div>
<div>
	<h3>Click the button to get my Name:</h3>
	<form method="post" action="/cgi-bin/do_shell.sh" class="formit">
		<input type="submit" class="button_submit" value="SHELL SCRIPT: ASCII ART">
	</form>
</div>
<div>
	<h3>Upload a File</h3>
	<form onsubmit="uploadFile(event)" class="formit">
			<input type="file" id="fileInput" name="file"/>
			<button type="submit" class="button_submit">Upload</button>
	</form>
</div>
<div>
	<h3>Enter the filename to delete and click the button:</h3>
	<form onsubmit="return sendDeleteRequest(event)" class="formit">
		<input type="text" id="filename" name="filename" placeholder="Enter filename">
		<input type="submit" class="button_submit" value="Delete File">
	</form>
</div>
</body>
</html>'''
print(page)