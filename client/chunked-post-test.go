package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strings"
	"iobuf"
)

func main() {

	rdr := bufio.NewReader(os.Stdin)
	fmt.Println("please introduce url")
	getURL, _ := rdr.ReadString('\n')
	getURL = strings.Replace(getURL, "\n", "", -1)
	
	fmt.Println("please introduce file path")
	getPath := rdr.ReadString('\n')
	getPath = strings.Replace(getPath, "\n", "", -1)	
	
	rd, wr := io.Pipe()

	client := &http.Client{Timeout: time.Duration(1) * time.Second}
	req, err := http.NewRequest("POST", postUri, rd)

	req.Header.Set("Content-Type", "image/jpeg")

	go func() {
		buf := make([]byte, 300)
		f, err := os.Open(filePath)
		if err != nil {
			log.Panic(err)
		}
		for {
			n, _ := f.Read(buf)
			if 0 == n {
				break
			}
			wr.Close()
		}
	}()

	resp, err := client.Do(req)
	if err != nil {
		log.Panic(err)
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Panic(err)
	}
	fmt.Println("[response body]")
	fmt.Println(string(body))
}
