package main

import (
	"encoding/json"
	"fmt"
	"github.com/go-telegram-bot-api/telegram-bot-api"
	"github.com/sevlyar/go-daemon"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"reflect"
	"strconv"
	"strings"
	"time"
	//	"github.com/tidwall/gjson"
)

var g_debug bool

const guest_log_filename = "guest.log"

func main() {

	var d_mode bool
	cntxt := &daemon.Context{
		PidFileName: "pid",
		PidFilePerm: 0644,
		LogFileName: "log",
		LogFilePerm: 0640,
		WorkDir:     "./",
		Umask:       027,
		//Args:        []string{"[go-daemon sample]"},
	}

	if argCount := len(os.Args[1:]); argCount >= 1 {
		fmt.Printf("Total Arguments (excluding program name): %d\n", argCount)
		//if arg := strconv.Atoi(os.Args[1]); arg == "-d" || arg == "--daemon" {
		if arg := os.Args[1]; arg == "-d" || arg == "--daemon" {
			d_mode = true
		}
	}

	fmt.Printf("daemon mode: %v\n", d_mode)

	if d_mode {
		d, err := cntxt.Reborn()
		if err != nil {
			log.Fatal("Unable to run: ", err)
		}
		if d != nil {
			return
		}
		defer cntxt.Release()

		log.Print("- - - - - - - - - - - - - - -")
		log.Print("daemon started")
	}
	FliGenBot()
}

func FliGenBot() {

	var b strings.Builder
	var bot_token string = os.Getenv("BOT_TOKEN")
	//Создаем бота
	bot, err := tgbotapi.NewBotAPI(bot_token)
	if err != nil {
		//panic(err)
		log.Panicln("cant't create tlg bot, specify BOT_TOKEN")
	}
	if len(os.Getenv("BOT_DEBUG")) > 0 {
		g_debug = true
	}

	bot.Debug = g_debug

	f, err := os.OpenFile(guest_log_filename, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0600)
	if err != nil {
		panic(err)
	}
	defer f.Close()

	fmt.Printf("BOT_DEBUG = %v\n", g_debug)
	log.Printf("Authorized on account %s", bot.Self.UserName)

	//Устанавливаем время обновления
	u := tgbotapi.NewUpdate(0)
	u.Timeout = 60

	b.Reset()
	b.WriteString("https://api.telegram.org/bot")
	b.WriteString(bot_token)
	var tlg_api_url string = b.String()

	b.Reset()
	b.WriteString("https://api.telegram.org/file/bot")
	b.WriteString(bot_token)
	var tlg_api_file_url string = b.String()

	//Получаем обновления от бота
	updates, err := bot.GetUpdatesChan(u)
	time.Sleep(time.Millisecond * 500)
	//	updates.Clear()

	for update := range updates {
		if update.Message == nil {
			continue
		}

		//Проверяем что от пользователья пришло именно текстовое сообщение
		if reflect.TypeOf(update.Message.Text).Kind() == reflect.String && update.Message.Text != "" {

			fmt.Printf("\n\n%v\n\n", update.Message.Text)

			rune_str := []rune(update.Message.Text)
			var cmd string = update.Message.Text
			var cmds []string = strings.Split(cmd, " ")
			var reply_str string
			var arg string

			if rune_str[0] == '/' {
				space_pos := strings.Index(update.Message.Text, " ")
				if space_pos > 0 {
					cmd = cmds[0]
				}
				if len(cmds) > 1 {
					arg = cmds[1]
				}
				log.Println("CMD:", cmd)
				log.Println("ARG:", arg)
			} else {
				reply_str := fmt.Sprintf("TODO: raw_msg:\n%v\n", cmd)
				log.Println(reply_str)
			}
			switch cmd {
			case "/gen":
				log.Println(cmd, arg)
				out, err := RunFliGen()
				if err != nil {
					log.Fatal(err)
					continue
				}
				log.Printf("output is %s\n", out)
				reply_str = string(out)
			case "/whoami":
				b.Reset()
				b.WriteString("You are: ")
				b.WriteString(update.Message.From.FirstName)
				id_str := strconv.Itoa(update.Message.From.ID)
				b.WriteString(id_str)
				reply_str = b.String()
			}
			msg := tgbotapi.NewMessage(update.Message.Chat.ID, reply_str)
			bot.Send(msg)

		} else {
			var curr_type = reflect.TypeOf(update.Message.Text).Kind()
			//b.WriteString(curr_type)
			log.Println(curr_type)
			b.Reset()
			b.WriteString(tlg_api_url)
			b.WriteString("/getFile?file_id=")
			b.WriteString(update.Message.Document.FileID)
			request := b.String()
			resp, err := http.Get(request)
			if err != nil {
				// handle error
				log.Println("error happened")
				log.Println(err.Error())
				continue
			}

			json_resp, err := ioutil.ReadAll(resp.Body)
			defer resp.Body.Close()
			if err != nil {
				log.Fatal(err)
				continue
			}
			b.Reset()
			b.Write(json_resp)
			var json_resp_str string = b.String()
			log.Printf("JSON RESPONSE: %s", json_resp_str)
			var info map[string]interface{}
			err_parse := json.Unmarshal([]byte(json_resp_str), &info)
			if err_parse != nil || info["ok"] != true {
				log.Println(err_parse)
				log.Printf("ok: %v", info["ok"])
				continue
			}
			file_name := info["result"].(map[string]interface{})["file_path"]
			log.Printf("file_path value: %s", file_name)
			b.Reset()
			b.WriteString(tlg_api_file_url)
			b.WriteString("/")
			b.WriteString(file_name.(string))
			file_url := b.String()
			log.Println(file_url)
			ss := strings.Split(file_name.(string), "/")
			file_name_loc := ss[len(ss)-1]
			log.Printf("File will be download to %s", file_name_loc)
			if err := DownloadFile(file_name_loc, file_url); err != nil {
				log.Println(err)
				continue
			}
			out, err := RunFliGen(file_name_loc)
			if err != nil {
				log.Fatal(err)
				continue
			}
			log.Printf("output is %s\n", out)
			reply_str := string(out)

			msg := tgbotapi.NewMessage(update.Message.Chat.ID, reply_str)
			bot.Send(msg)

		}
		whois_str := WhoIsIt(update, f)
		snitch_msg := tgbotapi.NewMessage(47241589, "Hey, Father! Someone tried to play with me :)\n"+whois_str)
		bot.Send(snitch_msg)
	}
}
func RunFliGen(file_path ...string) ([]byte, error) {
	const fli_gen_sh = "./fli-gen.sh"
	if l := len(file_path); l > 1 {
		args := make([]string, l)
		for i, elem := range file_path {
			args[i] = elem
		}
		return exec.Command(fli_gen_sh, args...).Output()
	} else {
		log.Println(fli_gen_sh)
		return exec.Command(fli_gen_sh).Output()
	}
}

func WhoIsIt(msg tgbotapi.Update, f *os.File) string {
	id := strconv.Itoa(msg.Message.From.ID)
	ret := msg.Message.From.FirstName + " " + msg.Message.From.LastName + " @" + msg.Message.From.UserName + "\nUser ID: " + id
	if _, err := f.WriteString(ret); err != nil {
		log.Println(err)
	}

	return ret
}

// DownloadFile will download a url to a local file. It's efficient because it will
// write as it downloads and not load the whole file into memory.
func DownloadFile(filepath string, url string) error {

	// Get the data
	resp, err := http.Get(url)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// Create the file
	out, err := os.Create(filepath)
	if err != nil {
		return err
	}
	defer out.Close()

	// Write the body to file
	_, err = io.Copy(out, resp.Body)
	return err
}
