#ifndef SMS_SENDER_H
#define SMS_SENDER_H

#include <string>

class SmsSender {
public:
    SmsSender(const std::string& accountSid, const std::string& authToken, const std::string& fromNumber);
    bool sendSms(const std::string& toNumber, const std::string& message);

private:
    std::string accountSid;
    std::string authToken;
    std::string fromNumber;
};

#endif // SMS_SENDER_H
