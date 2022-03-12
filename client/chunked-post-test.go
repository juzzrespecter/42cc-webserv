package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strings"
)

func getPath(uri string) string {
	slash := strings.LastIndex(uri, "/")
	if slash == -1 {
		log.Panic("no file to upload")
	}
	return uri[slash:]
}

func main() {
	if len(os.Args[:1]) != 1 {
		log.Panic("wrong number of args.")
	}
	postUri := os.Args[1]
	filePath := getPath(postUri)
	rd, wr := io.Pipe()

	client := &http.Client{ /*Timeout:*/ }
	req, err := http.NewRequest("POST", postUri, rd)

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
