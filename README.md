# IMAQLIQ_task
Simple client-server app

## Dependencies
* cmake

## Instructions
```bash
chmod +x ./run.sh
./run.sh
```
or

```bash
chmod +x ./run.sh
./run.sh -k <number signal>
```


## Task

Клиент-серверное приложение под Linux. \
Клиент - программа, запускаемая из консоли. \
Сервер - демон, корректно завершающийся по сигналам SIGTERM и SIGHUP. \
Клиент должен передать содержимое текстового файла через TCP. \
Сервер должен принять и сохранить в файл.

