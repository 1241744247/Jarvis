#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include <json/json.h>
#include <fstream>
#include <fcntl.h>
#include <cstdio>
#include <unordered_map>
#include "base/http.h"
#include "speech.h"
#define ASR_PATH "temp_file/demo.wav"
#define PLAY_PATH "temp_file/play.wav"
#define CMD_PATH "command.etc"

class Util{
public:
  static bool Exec(std::string command , bool is_print)
  {
    if(!is_print)
    {
      command += ">/dev/null 2>&1";
    }
    FILE *fp=popen(command.c_str(),"r");
    if(nullptr==fp)
    {
      std::cerr<<"popen /"<<command<<"/ error"<<std::endl;
      return false;
    }
    if(is_print)
    {
      char c;
      while(fread(&c,1,1,fp)>0)
      {
        fwrite(&c,1,1,stdout);
      }
    }
    pclose(fp);
    return true;
  }
};

class Robot{
private:
  std::string apiKey="bfeccfe688f642169ac2e9a690236fa2";
  std::string userId="Iron";
  std::string url="http://openapi.tuling123.com/openapi/api/v2";
  aip::HttpClient client;
private:
  bool IsCodeLegal(int code)
  {
    bool result = true;
    switch(code)
    {
      case 5000:
        result = false;
        break;
      case 6000:
        result = false;
        break;
      case 4000:
        result = false;
        break;
      case 4001:
        result = false;
        break;
      case 4002:
        result = false;
        break;
      case 4003:
        result = false;
        break;
      case 4005:
        result = false;
        break;
      case 4007:
        result = false;
        break;
      case 4100:
        result = false;
        break;
      case 4200:
        result = false;
        break;
      case 4300:
        result = false;
        break;
      case 4400:
        result = false;
        break;
      case 8008:
        result = false;
        break;
      default:
        break;
        
    }
    return result;
  }


public:
  std::string  MessageToJson(std::string message)
  {
    Json::Value root;
    Json::StreamWriterBuilder wb;
    std::ostringstream os;
    std::unique_ptr<Json::StreamWriter> sw (wb.newStreamWriter());
    Json::Value userInfo;
    Json::Value perception;
    Json::Value inputText;
    inputText["text"]=message;
    perception["inputText"]=inputText;
    userInfo["apiKey"]=apiKey;
    userInfo["userId"]=userId;
    root["reqType"]=0;
    root["userInfo"]=userInfo;
    root["perception"]=perception;
    sw->write(root,&os);
    
    std::string jsonmessage=os.str();
    return jsonmessage;
  }
  std::string  RequestTL(std::string request)
  {
    std::string response;
    client.post(url,nullptr,request,nullptr,&response);
    return response;
  }
  std::string JsonToMessage(std::string jsonmessage)
  {
    std::string echomessage;
    int code;
    JSONCPP_STRING errs;
    Json::Value root;
    Json::CharReaderBuilder rb;
    std::unique_ptr<Json::CharReader> const cr(rb.newCharReader());
    bool ret = cr->parse(jsonmessage.data(),jsonmessage.data()+jsonmessage.size(),&root,&errs);
    if(!ret||!errs.empty())
    {
      std::cerr<<"JsonToMessage Error "<<errs<<std::endl;
    }
    Json::Value intent=root["intent"];
    code=intent["code"].asInt();
    Json::Value results=root["results"][0];
    Json::Value values=results["values"];
    echomessage=values["text"].asString();
    if(IsCodeLegal(code))
      return echomessage;
    else 
    {
      std::cerr<<"JsonToMessage error"<<std::endl;
      return "";
    }
  }
public:
  Robot()
  {
 
  }
  ~Robot()
  {

  }

};
class SpeechRec{
private:
  std::string app_id="16879625";
  std::string api_key="GDLWQyehmWgCkck7wP2RdSvt";
  std::string secret_key="o8KXFd661jEBOYpSIL1Vvt4WoFQrocfj";
  aip::Speech *client;
private:
  bool IsErrLegal(int err_no)
  {
    bool result = false;
    switch(err_no)
    {
      case 3300:
        break;
      case 3301:
        break;
      case 3302:
        break;
      case 3303:
        break;
      case 3304:
        break;
      case 3305:
        break;
      case 3307:
        break;
      case 3308:
        break;
      case 3309:
        break;
      case 3310:
        break;
      case 3311:
        break;
      case 3312:
        break;
      case 0:
        result = true;
        break;
      default:
        break;
        
    }
    return result;
  }
  
public:
  SpeechRec()
  {
    client=new aip::Speech(app_id,api_key,secret_key);
  }
  bool ASR(std::string &out)
  {
    std::map<std::string, std::string> options;
    options["dev_pid"] = "1536";
    std::string file_content;
    aip::get_file_content(ASR_PATH, &file_content);
    Json::Value result = client->recognize(file_content, "wav", 16000, options);
    int err_no=result["err_no"].asInt();
    if(IsErrLegal(err_no))
    {
      out=result["result"][0].asString();
      return true;
    }
    else 
    {
      return false;
    }
  }
  bool TTS(std::string text)
  {
    std::ofstream ofile;
    std::string file_ret;
    std::map<std::string, std::string> options;
    options["spd"] = "5";
    options["per"] = "103";
    options["vol"] = "5";
    options["aue"] = "6";
    ofile.open(PLAY_PATH, std::ios::out | std::ios::binary);
    Json::Value result = client->text2audio(text.c_str(), options, file_ret);
    if (!file_ret.empty())
    {
      ofile << file_ret;
      return true;
    } 
    else 
    {
      //服务端合成错误
      std::cerr<<"语言合成错误" << result.toStyledString()<<std::endl;   
      return false;
    }
    ofile.close();
  }
  ~SpeechRec()
  {
    delete client;
  }
};




class Jarvis{
private:
  Robot jvs;
  SpeechRec sr;
  std::string message;
  std::unordered_map<std::string,std::string> command_map;
private:
  static void* Progress(void *arg)
  {
    pthread_detach(pthread_self());
    char buf[53]={0};
    const char* lable="|/-\\";
    for(int i=0;i<=50;++i)
    {
      printf("[Record..][%-51s][%3d%%][%c]\r",buf,i*2,lable[i%4]);
      fflush(stdout);
      buf[i]='=';
      buf[i+1]='>';
      usleep(100000);
    }
    std::cout<<std::endl;  
  }
  void ProgressBarRun()
  {
    pthread_t tid;
    if(pthread_create(&tid,nullptr,Progress,nullptr)!=0)
    {
      std::cerr<<"Pthread creat failed! ! !"<<std::endl;
    }
  }

  bool Record()
  {
    //std::cout <<"debug:"<<"Recording.."<<std::endl;
    std::string command="arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE ";
    command += ASR_PATH;
    bool ret =  Util::Exec(command,false);
    //std::cout <<"debug:"<<"Record  over!!"<<std::endl; 
    return ret;
  }
  bool CommandInit()
  {
    char buffer[256];
    std::string k;
    std::string v;
    std::ifstream in(CMD_PATH);
    bool ret=false;
    if(!in.is_open())
    {
      std::cerr << "open file error" <<std::endl;
      exit(1);
    }
    std::string sep = ":";
    while(in.getline(buffer, sizeof(buffer)))
    {
      std::string str = buffer;
      std::size_t pos = str.find(sep);
      if(std::string::npos == pos)
      {
        std::cerr << "Load Etc Error" << std::endl;
        continue;
      }
      k = str.substr(0, pos);
      v = str.substr(pos+sep.size());
      k+="。";
      command_map.insert(std::make_pair(k, v));
      //
      ret = true;
    }
    in.close();
    return ret;
  }
  bool MessageIsCommand(std::string _message, std::string &cmd)
  {
    auto it = command_map.find(_message);
    if(it==command_map.end())
    {
      cmd="";
      return false;
    }
    else 
    {
      cmd=it->second;
      return true;
    }
  }
public:
  void Run()
  {
    volatile bool stop = false;
    while(!stop)
    {
      if(!CommandInit())
      {
        std::cerr<<"CommandInit failed"<<std::endl;
        return ;
      }
      ProgressBarRun();
      if(Record())
      {
        if(sr.ASR(message))
        {
          std::cout<<"IronMan :"<<message<<std::endl;
          std::string echo_message;
          std::string cmd;
          if(MessageIsCommand(message,cmd))
          {
            Util::Exec(cmd,true);
            continue;
          }
          else 
          {
            if(message=="你走吧。")
            {
              echo_message="那我走了，不要想我哦。";
              stop=true;
            }
            else 
              echo_message=jvs.JsonToMessage(jvs.RequestTL(jvs.MessageToJson(message)));

            std::cout<<"Jarvis :"<<echo_message<<std::endl;
            
          }
          std::string play = "cvlc --play-and-exit ";//播放完毕退出：--play-and-exit
          play += PLAY_PATH;
          play += " >/dev/null 2>&1";
          sr.TTS(echo_message); //语音合成
          Util::Exec(play, false); //执行播放
        }
        else 
        {
          std::cerr<<"ASR error"<<std::endl; 
        }
      }
      else 
      {
        std::cerr<<"Record failed"<<std::endl;
      }
    }
  }
  Jarvis()
  {

  }
  ~Jarvis()
  {

  }

};
