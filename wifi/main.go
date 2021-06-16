package main

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"os"
	"strings"
	"sync"
	"time"
)

type status_t struct {
	lock               sync.RWMutex
	lastConnected      time.Time
	underAttack        bool
	somethingWentWrong bool
}

func (s *status_t) getUnderAttack() (ret bool) {
	s.lock.RLock()
	ret = s.underAttack
	s.lock.RUnlock()
	return
}

func (s *status_t) setLastConnected(t time.Time) {
	s.lock.Lock()
	s.lastConnected = t
	s.lock.Unlock()
}

func (s *status_t) getLastConnected() (ret time.Time) {
	s.lock.RLock()
	ret = s.lastConnected
	s.lock.RUnlock()
	return
}

func (s *status_t) getSomethingWentWrong() (ret bool) {
	s.lock.RLock()
	ret = s.somethingWentWrong
	s.lock.RUnlock()
	return
}

func (s *status_t) update() {
	s.lock.Lock()
	if s.somethingWentWrong = time.Now().After(s.lastConnected.Add(50 * time.Second)); s.somethingWentWrong {
		log.Println("** SOMETHING WENT WRONG **")
	} else if s.underAttack = time.Now().After(s.lastConnected.Add(40 * time.Second)); s.underAttack {
		log.Println("!! under attack !!")
	}
	s.lock.Unlock()
}

var status status_t

const clientIP = "10.88.15.66"
const routerIP = "10.88.5.23"
const adminIP1 = "10.5.2.199"
const FLAG = "HW5{j0e_ts4I_1s_d0ub1e_gun_k4i's_b3st_fr13nD}"

// ---------------------------------------

func main() {
	status = status_t{}
	status.setLastConnected(time.Now())
	go func() {
		for {
			status.update()
			time.Sleep(1 * time.Second)
		}
	}()

	logFile, err := os.OpenFile("log.txt", os.O_CREATE|os.O_APPEND|os.O_RDWR, 0666)
	if err != nil {
		panic(err)
	}
	mw := io.MultiWriter(os.Stdout, logFile)
	log.SetOutput(mw)

	go webServer()
	tcpServer()
}

func tcpServer() {
	ln, err := net.Listen("tcp", ":6887")
	if err != nil {
		log.Panic(err)
	}
	log.Println("[TCP] listening ...")
	for {
		conn, err := ln.Accept()
		if err != nil {
			// handle error
		}
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	buf := make([]byte, 1000)
	n, _ := conn.Read(buf)
	if strings.HasPrefix(conn.RemoteAddr().String(), clientIP) {
		log.Println("[TCP] connection from CLIENT", conn.RemoteAddr(), ":", string(buf[:n]))
		status.setLastConnected(time.Now())
	} else {
		log.Println("[TCP] connection from", conn.RemoteAddr(), ":", string(buf[:n]))
	}
	conn.Write([]byte("OK.\n"))
	conn.Close()
}

func webServer() {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		if status.getSomethingWentWrong() {
			log.Println("[HTTP] connect from", r.RemoteAddr, ", send something went wrong")
			fmt.Fprintln(w, "This page is temporarily unavailable to prevent cheating.\nIf there isn't anyone else doing this task *right now*, then something just went wrong. Please contact TA to fix it.")
			return
		}

		if !strings.HasPrefix(r.RemoteAddr, routerIP) {
			fmt.Fprintln(w, "You are not at the battle field! Bad!")
			log.Println("[HTTP] invalid connect from", r.RemoteAddr)
		} else if status.getUnderAttack() {
			fmt.Fprintln(w, FLAG)
			log.Println("[HTTP] connect from router, give flag")
		} else {
			life := 40 - int64(time.Now().Sub(status.getLastConnected())/time.Second)
			fmt.Fprintln(w, "<html><head><script>setTimeout(function(){window.location.reload(1);},1000);</script></head><body>")
			fmt.Fprintln(w, "Life of bug:", life, "</body></html>")
			log.Println("[HTTP] connect from router, life of bug:", life)
		}
	})

	http.HandleFunc("/log.txt", func(w http.ResponseWriter, r *http.Request) {
		if strings.HasPrefix(r.RemoteAddr, adminIP1) {
			log.Println("[HTTP] admin access log.txt")
			dat, err := ioutil.ReadFile("log.txt")
			if err != nil {
				fmt.Fprintln(w, err)
			} else {
				fmt.Fprintln(w, string(dat))
			}
		} else {
			w.WriteHeader(http.StatusForbidden)
			log.Println("[HTTP] invalid connection from", r.RemoteAddr, ": access log.txt")
		}
	})

	log.Fatal(http.ListenAndServe(":18080", nil))
}
