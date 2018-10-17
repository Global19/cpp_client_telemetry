#pragma once

#include "pal/PAL.hpp"
#include <map>
#include <string>
#include <mutex>

namespace ARIASDK_NS_BEGIN {

class KillSwitchManager 
{
public:

	KillSwitchManager(): m_isRetryAfterActive(false), m_retryAfterExiryTime(0)
	{
	}

	virtual ~KillSwitchManager() 
	{
	}

	void handleResponse(HttpHeaders& headers)
	{
        std::string timeStr = headers.get("Retry-After");
        if (!timeStr.empty())
        {
            int64_t timeinSecs = std::stoi(timeStr);
            if (timeinSecs > 0)
            {
                std::lock_guard<std::mutex> guard(m_lock);
                m_retryAfterExiryTime = PAL::getUtcSystemTime() + timeinSecs; 
                m_isRetryAfterActive = true;
            }
        }

		std::pair<std::multimap<std::string, std::string>::const_iterator, std::multimap<std::string, std::string>::const_iterator> ret;
		ret = headers.equal_range("kill-tokens");

		if (ret.first != ret.second)
		{
			std::vector<std::string> killtokensVector;

			for (std::multimap<std::string, std::string>::const_iterator it = ret.first; it != ret.second; ++it)
			{
				killtokensVector.push_back(it->second);
			}

			int64_t timeinSecs = 0;
			std::string timeString = headers.get("kill-duration");
			if (!timeString.empty())
			{
				timeinSecs = std::stoi(timeString);
			}

			if (killtokensVector.size() > 0 && timeinSecs > 0)
			{
				for (std::vector<std::string>::iterator iter = killtokensVector.begin(); iter < killtokensVector.end(); ++iter)
				{
					addToken(*iter, timeinSecs);
				}
			}
		}

	}

	void addToken(const std::string& tokenId, int64_t timeInSeconds)
	{
		std::lock_guard<std::mutex> guard(m_lock);
		if (timeInSeconds > 0)
		{
			m_tokenTime[tokenId] = PAL::getUtcSystemTime() + timeInSeconds; //convert milisec to sec
		}
	}

	bool isTokenBlocked(const std::string& tokenId)
	{
		std::lock_guard<std::mutex> guard(m_lock);

        if (m_isRetryAfterActive)
        {
            if(m_retryAfterExiryTime > PAL::getUtcSystemTime())  
            {
                return true;//always return true for all tokens
            }
            else
            {
                m_retryAfterExiryTime = 0;
                m_isRetryAfterActive = false;
            }
        }
		std::map<std::string, int64_t>::iterator iter = m_tokenTime.find(tokenId);
		if (iter != m_tokenTime.end())
		{//found, check the time stamp
			int64_t timeStamp = m_tokenTime[tokenId];
			
			if (timeStamp > PAL::getUtcSystemTime())  //convert milisec to sec
			{
				return true;
			}
			else
			{ //remove the entry for this token as this has expired
				m_tokenTime.erase(tokenId);
			}			
		}
		
		return false;
	}

	void removeToken(const std::string& tokenId)
	{
		std::lock_guard<std::mutex> guard(m_lock);
		std::map<std::string, int64_t>::iterator iter = m_tokenTime.find(tokenId);
		if (iter != m_tokenTime.end())
		{//found, check the time stamp
			m_tokenTime.erase(tokenId);
		}
	}

    bool isRetryAfterActive()
    {
        return m_isRetryAfterActive;
    }

private:
	std::map<std::string, int64_t> m_tokenTime;
	std::mutex		m_lock;
    bool            m_isRetryAfterActive;
    int64_t         m_retryAfterExiryTime;
};

} ARIASDK_NS_END
