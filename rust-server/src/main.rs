use std::{fs, io::{BufRead, BufReader, Write}, net::{TcpListener, TcpStream}};

fn main() {
    let listener = TcpListener::bind("127.0.0.1:3000").unwrap();    // unwrap to not handle errors

    for stream in listener.incoming() {
        let stream = stream.unwrap();
        handle_connections(stream);
        println!("Connection established!");
    }
}

fn handle_connections(mut stream: TcpStream) {
    let buf_reader = BufReader::new(&mut stream);
    let request_line = match buf_reader.lines().next() {
        Some(Ok(line)) => line,
        Some(Err(e)) => {
            eprintln!("Error reading line: {}", e);
            return; 
        },
        None => {
            eprintln!("No lines in buffer");
            return;
        }
    };
    
    if request_line == "GET / HTTP/1.1" {
        let response: &str = "HTTP/1.1 200 OK";
        let contents = fs::read_to_string("index.html").unwrap();
        
        let length = contents.len();

        let response =
            format!("{response}\r\nContent-Type: text/html\r\nContent-Length: {}\r\n\r\n{}", length, contents);

        stream.write(response.as_bytes()).unwrap();
        stream.flush().unwrap();
    } else {
        // other request 
    }
    

}