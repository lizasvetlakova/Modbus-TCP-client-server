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

Для сборки приложений в терминале: sh build.sh

Если клиент отключается, сервер продолжает работать. 
Если сервер отключается, то клиент пробует восстановить соединение. Если сервер не отвечает в течении некоторого времени, то клиент завершает работу.
