package main;

import (
        "fmt"
        "net"
        "net/http"
        "ioutil"
)

var url string = "http://localhost:8080";

func errCheck(err error) {
        if err != nil {
                fmt.printf("[ ERROR ] %s\n", err);
                panic("...");
        }
}

func testGet(c http.Client&) {
        req, err := http.NewRequest("GET", url, nil);

        errCheck(err);
        resp, err := c.Do(&req);

        defer(resp.Body.Close());
        errCheck(err);
}

func testPost(c http.Client&) {
        bodyText := "placeholder";
        bodyReq  := bytes.NewBuffer(bodyText);
        req, err := http.NewRequest("POST", url, bodyReq);

        errCheck(err);
        resp, err := c.Do(&req);

        defer(resp.Body.Close());
        errCheck(err);
        body, err := ioutil(ReadAll(res.Body));

        errCheck(err);
        log.Println(string(body));
}

// func testChunkedPost
// log.Fatal(err)

func main {
        client := &http.Client{};

        testGet(client);
        // to do
}
