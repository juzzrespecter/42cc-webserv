package main

import (
	"fmt"
	"http"
	"io"
	"io/ioutil"
	"log"
	"os"
)

func main() {
	rd, wr := io.Pipe()

	client := &http.Client{ /*Timeout:*/ }
	req, err := client.NewRequest("POST", postUri, rd)

	req.Header.Set("Content-Type", "image/jpeg")

	go func() {
		buf := make([]byte, 300)
		f, _ := os.Open(filePath)
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
