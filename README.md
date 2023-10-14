# Modbus-TCP-client-server
Два консольных приложения: TCP/IP клиент и сервер, которые обмениваются сообщениями в формате Modbus.

Сервер:
1) содержит 10 регистров хранения,
2) имеет заданный Modbus адрес 3 и TCP порт 2000,
3) поддерживает несколько клиентов, задано максимальное количество клиентов - 50, 
4) обрабатывает запросы и посылает значения регистров или сообщение об ошибке.

Клиент:
1) устанавливает соединение с сервером, 
2) имеет заданный ip адрес сервера 127.0.0.1 и TCP порт 2000
3) отправляет Modbus адрес сервера, номер первого регистра и количество запрашиваемых регистров,
4) выводит полученный ответ.

Для запуска приложений в терминале: gcc client.c -o client (аналогично с сервером)

TODO:
1)	Конфигурационный файл 
2)	Клиент отключается -- сервер тоже. И наоборот. Восстановление клиентом соединения, после разрыва.
3)	Если включить сервер, включить клиента, отключить сервер -- то потом запустить сервер больше не получится. При запуске сервера будет писаться: Port is unavailable...
