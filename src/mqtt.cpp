#include <iostream>
#include <thread>
#include <unistd.h>
#include "mosquitto.h"
#include "mqtt.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

void my_thread(void *, bool &done)
{
  // pop (free copy?)
}

void on_connect(struct mosquitto *, void *, int)
{
  std::cout << "Connected" << std::endl;
}

void mosq_cleanup(mosquitto *mosq)
{
  mosquitto_destroy(mosq);
  mosquitto_lib_cleanup();
}

void on_message(struct mosquitto *, void *, const struct mosquitto_message *message)
{
  // std::string json_payload(reinterpret_cast<char *>(message->payload), message->payloadlen);
  // push (copy?)

  Document jd;
  jd.Parse(reinterpret_cast<char *>(message->payload), message->payloadlen);
  std::cout << message->topic << " time " << jd["time"].GetString() << std::endl;
}

// https://mosquitto.org/api/files/mosquitto-h.html
int main(int argc, char **argv)
{
  int major, minor, revision;
  struct mosquitto *mosq = NULL;

  int opt;
  std::string hostname;
  std::string username;
  std::string password;

  while ((opt = getopt(argc, argv, "hs:u:p:")) != -1)
  { // for each option...
    switch (opt)
    {
    case 'h':
      usage(argv);
      exit(EXIT_SUCCESS);
      break;
    case 's':
      hostname = std::string(optarg);
      break;
    case 'u':
      username = std::string(optarg);
      break;
    case 'p':
      password = std::string(optarg);
      break;
    }
  }

  if (username.empty() || password.empty() || hostname.empty())
  {
    usage(argv);
    exit(EXIT_FAILURE);
  }

  mosquitto_lib_version(&major, &minor, &revision);
  std::cout << "Libmosquitto version: " << major << "." << minor << "." << revision << std::endl;

  if (mosquitto_lib_init() != MOSQ_ERR_SUCCESS)
  {
    std::cerr << "Error: failed to initialize mosquitto library" << std::endl;
    exit(EXIT_FAILURE);
  }

  mosq = mosquitto_new(NULL, true, nullptr);
  if (!mosq)
  {
    std::cerr << "Error: failed to create mosquitto client" << std::endl;
    exit(EXIT_FAILURE);
  }

  if (mosquitto_username_pw_set(mosq, username.c_str(), password.c_str()) != MOSQ_ERR_SUCCESS)
  {
    std::cerr << "Error: failed to set username and password" << std::endl;

    mosq_cleanup(mosq);
    exit(EXIT_FAILURE);
  }

  // Connect to MQTT broker
  if (mosquitto_connect(mosq, hostname.c_str(), 1883, 60) != MOSQ_ERR_SUCCESS)
  {
    std::cerr << "Error: connecting to MQTT broker failed" << std::endl;

    mosq_cleanup(mosq);
    exit(EXIT_FAILURE);
  }

  mosquitto_message_callback_set(mosq, on_message);
  mosquitto_connect_callback_set(mosq, on_connect);

  // Subscribe to any channel that ends with ambient_data
  mosquitto_subscribe(mosq, NULL, "rtl_433/#", 0);
  if (mosquitto_loop_start(mosq) != MOSQ_ERR_SUCCESS)
  {
    std::cerr << "Error: failed to start mosquitto loop" << std::endl;

    mosq_cleanup(mosq);
    exit(EXIT_FAILURE);
  }

  bool done = false;
  std::thread t1(my_thread, nullptr, std::ref(done));

  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  mosquitto_loop_stop(mosq, true);

  mosq_cleanup(mosq);

  return 0;
}

void usage(char **argv)
{
  fprintf(stderr, "Usage: %s [-h] [-s host] [-u user] [-p password]\n", argv[0]);
}
