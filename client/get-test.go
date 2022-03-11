package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"strings"
	"time"
)

func main() {
	client := &http.Client{Timeout: time.Duration(1) * time.Second}

	rdr := bufio.NewReader(os.Stdin)
	fmt.Println("Enter request URI:")
	getUri, _ := rdr.ReadString('\n')
	getUri = strings.Replace(getUri, "\n", "", -1)

	req, err := http.NewRequest("GET", getUri, nil)
	if err != nil {
		log.Panic(err)
	}
	resp, err := client.Do(req)
	if err != nil {
		log.Panic(err)
	}
	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Panic(err)
	}
	fmt.Println("[response]")
	fmt.Printf("%s\n", body)
}
