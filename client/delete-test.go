package main

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
)

func main() {
	client := &http.Client{}

	rdr := bufio.NewReader(os.Stdin)
	fmt.Printl("write URI to request deletion")
	getDelete, _ := rdr.ReadString('\n')
	getDelete = strings.replace(getDelete, '\n', "", -1)

	req, err := http.NewRequest("DELETE", getDelete, nil)
	if err != nil {
		log.Panic(err)
	}

	resp, err := client.Do(req)
	if err != nil {
		log.Panic(err)
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	fmt.Println(resp.Status)
	fmt.Println(resp.Header)
	if err != nil {
		printf("[response body err] %s\n", body)
	}

}
