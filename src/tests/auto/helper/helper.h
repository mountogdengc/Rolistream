#include <algorithm>

#include "network/networkmessage.h"
#include <QByteArray>
#include <QStringList>
#include <utility>

class QObject;
namespace Helper
{
template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
    /* Credits: Mark Nelson http://marknelson.us */
    if((first == last) || (first == k) || (last == k))
        return false;
    Iterator i1= first;
    Iterator i2= last;
    ++i1;
    if(last == i1)
        return false;
    i1= last;
    --i1;
    i1= k;
    --i2;
    while(first != i1)
    {
        if(*--i1 < *i2)
        {
            Iterator j= k;
            while(!(*i1 < *j))
                ++j;
            std::iter_swap(i1, j);
            ++i1;
            ++j;
            i2= k;
            std::rotate(i1, j, last);
            while(last != j)
            {
                ++j;
                ++i2;
            }
            std::rotate(k, i2, last);
            return true;
        }
    }
    std::rotate(first, k, last);
    return false;
}

class TestMessageSender : public MessageSenderInterface
{
public:
    TestMessageSender();
    virtual void sendMessage(const NetworkMessage* msg) override;

    QByteArray messageData() const;

private:
    QByteArray m_msgData;
};

std::pair<bool, QStringList> testAllProperties(QObject* obj);
} // namespace Helper
