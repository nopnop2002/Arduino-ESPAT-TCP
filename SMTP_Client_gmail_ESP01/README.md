# Environment
You need to change the following environment.

- #define SMTP_SERVER "smtp.gmail.com"   
SMTP Server   
- #define SMTP_PORT   465   
SMTP Server port   
- #define BASE64_USER "Base64 Encorded Your Username of gmail"   
Gmain user name converted to BASE64   
See below   
- #define BASE64_PASS "Base64 Encorded Your Password of gmail   
Gmail Password converted to BASE64   
See below   
- #define MAIL_FROM   "mailFrom@gmail.com"   
Mail from   
- #define MAIL_TO     "mailTo@provider.com"   
Mail To   
- #define JAPANESE    0   
1 to send Japanese text   


# How to convert mail address and password.

You can convert your mail address of gmail and a password to a character string of Base64 using this page.

https://www.base64encode.org/

For example when your mail address is "aaa@gmail.com", enter "aaa@gmail.com" and press a ENCODE button.    
"YWFhQGdtYWlsLmNvbQ==" is shown to a lower.   
You use this as a mail address.   

About a password, you can convert by the same procedure.   
