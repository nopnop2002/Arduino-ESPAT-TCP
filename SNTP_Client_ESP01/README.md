# Environment
You need to change the following environment.

- #define SNTP_SERVER     "time.google.com"   
SNTP server you use   
- #define TIME_ZONE       9   
Your local time zone   


# Using terminal software such as CoolTerm
```
AT+CIPDNS_CUR?
+CIPDNS_CUR:192.168.10.1
+CIPDNS_CUR:208.67.222.222

OK

AT+CIPDNS_CUR=1,"8.8.8.8","8.8.4.4"

OK

AT+CIPDNS_CUR?
+CIPDNS_CUR:8.8.8.8
+CIPDNS_CUR:8.8.4.4

OK

AT+CIPSNTPCFG=1,9,"time.google.com"

OK

AT+CIPSNTPTIME?
+CIPSNTPTIME:Tue May 07 20:00:11 2024
OK
```
