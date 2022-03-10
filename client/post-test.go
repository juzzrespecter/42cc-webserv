package main

import (
	"fmt"
	"os"
	"http"
	"io/ioutil"
	"time"
)

// var urlPost = "http://localhost:8083"
// get url from args ???

func main() {
	if len(os.Args) != 3 {
		log.Panic("wrong number of arguments")
	}
	url := os.Args[1]
	filePath := os.Args[2]
	fileString, err := ioutil.ReadFile(filePath)
	if err != nil {
		log.Pannic(err)
	}
	client := &http.Client{Timeout: time.Duration(1) * time.Second}
	// CheckRedirect (type func(req *Request, via []*Request))
	// Jar (type CookieJar)
	
	// bytes.NewBuffer([]byte)
	req, err := http.NewRequest("POST", urlPost, bytes.NewBuffer(fileString)) // body requires implemented io.Reader
	if err != nil {
		log.Panic(err) // call to Print; then Exit(1)
	}
	reqBody, err := ioutil.ReadAll(req.Body)
	if err != nil {
		log.Panic(err)
	}
	fmt.Println("[ request to send ]")
	fmt.Printf("%s\n", reqBody)
	// adding Headers
	//		Header == map[string][]string
	req.Header.Add("Accept", `application/json`)
	resp, err := client.Do(req)
	if err != nil {
		log.Panic(err)		
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body) // reads until error or EOF and returns read data
	if err != nil {
		log.Panic(err)
	}
	fmt.Println("[ response received from Server ]")
	fmt.Printf("%s\n", resp.Status)
	fmt.Printf("%s\n", body)
}