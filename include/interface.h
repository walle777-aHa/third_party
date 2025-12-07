
#ifndef SPACECRAFT_DATA_INTERFACE_H
#define SPACECRAFT_DATA_INTERFACE_H

#include <string>
#include <functional>

namespace spacecraft {
namespace bluespace {

class IData {
public:
    /*
     * Get data's ID.
     */
    virtual int64_t get_id() = 0;

    /*
     * Set data's ID.
     */
    virtual void set_id(int64_t seq) = 0;

    /*
     * Get data's timestamp.
     */
    virtual uint64_t get_timestamp() = 0;

    /*
     * Set data's timestamp.
     */
    virtual void set_timestamp(uint64_t t) = 0;

    /*
     * Get data's original timestamp.
     */
    virtual uint64_t get_original_timestamp() = 0;

    /*
     * Set data's original timestamp.
     */
    virtual void set_original_timestamp(uint64_t t) = 0;

    /*
     * Clear content of data.
     */
    virtual void clear() = 0;

    virtual std::string data_type() const = 0;
    virtual std::string definition() const = 0;

    virtual bool serialize_tostring(std::string* str) const  = 0;
    virtual bool serialize_totext(std::string* str) const  = 0;
    virtual bool parse_fromstring(const std::string& str) = 0;
    virtual bool parse_fromtext(const std::string& str) = 0;
    virtual size_t serialized_bytesize() const = 0;

};

typedef std::function<int(IData*, IData*)> LIB_process;
typedef std::function<int(const std::string&)> LIB_init;
typedef std::function<int()> LIB_release;

class IExec {
public:

    virtual int process(IData* input, IData* output) = 0;
    virtual int init(const std::string& config_dir) = 0;
    virtual int release() = 0;
};

class IPublish {
public:
    IPublish() = default;
    virtual ~IPublish() {}

    virtual bool publish(const std::string& topic, const std::string& message) = 0;
};

class ISubscribe {
public:
    ISubscribe() = default;
    virtual ~ISubscribe() {}

    virtual bool subscribe(const std::string& topic, std::function<void(const std::string&)> callback_func) = 0;
};

}
}

#endif //SPACECRAFT_DATA_INTERFACE_H
