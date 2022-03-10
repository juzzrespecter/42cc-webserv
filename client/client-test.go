package main

import (
	"bytes"
	"fmt"
	"net/http"
	"os"
)

// test GET
// test POST
// test POST chunked
// test DELETE
// test bigPOST

var url string = "http://localhost:8000/"

func errCheck(err error) {
	if err != nil {
		fmt.Printf("[ ERROR ] %s\n", err)
		panic("fatal error")
	}
}

func testGet(c http.Client) {
	resp, err := c.Get(url)

	errCheck(err)
	defer resp.Body.Close()

}

func testPost(c http.Client) {

}

func main() {
	testArgs := os.Args[1:]

	if len(testArgs) != 1 {

	
		

func main() {
	client := http.Client{}

	pathToFile := "./test_file"
	//bodyText := []byte("this is the body of the request.");

	bodyText, err := os.ReadFile(pathToFile)
	if err != nil {
		fmt.Printf("[ ERROR ] %s\n", err)
		return
	}
	bodyReq := bytes.NewBuffer(bodyText)

	req, err := http.NewRequest("POST", url, bodyReq)
	if err != nil {
		fmt.Printf("[ ERROR ] %s\n", err)
		return
	}
	//req.Header.Set("Transfer-Encoding", "chunked");
	//req.TransferEncoding = []string{"chunked"};
	//req.ContentLength = 0;

	//response, err := client.Get("http://localhost:8000/");
	//response, err := client.Post(url, "text/html", bodyResp);
	response, err := client.Do(req)
	if err != nil {
		fmt.Printf("[ ERROR ] %s\n", err)
		return
	}
	fmt.Println("Sent request")
	defer response.Body.Close()
	//body, err := ioutil.ReadAll(response.Body);
	//fmt.Printf("Body: %s\n", body);
	fmt.Printf("Status: %s\n", response.Status)

	return
}
