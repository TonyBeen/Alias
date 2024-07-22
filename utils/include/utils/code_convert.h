/*************************************************************************
    > File Name: code_convert.h
    > Author: hsz
    > Brief:
    > Created Time: 2024年07月19日 星期五 16时44分18秒
 ************************************************************************/

#ifndef __EULAR_CODE_CONVERT_H__
#define __EULAR_CODE_CONVERT_H__

#include <stdint.h>
#include <string>

namespace eular {

class CodeConvert
{
    CodeConvert(const CodeConvert &) = delete;
    CodeConvert &operator=(const CodeConvert &) = delete;

public:
    enum CodeFlag {
        GBK = 0,
        GB2312,
        UTF8,
        UTF16LE,
        UTF16BE,
    };

    CodeConvert();
    ~CodeConvert() = default;

    /**
     * @brief 编码转换开始
     * 
     * @param from 当前字符串编码方式
     * @param to 转成哪种编码方式
     * @return true 
     * @return false 
     */
    bool convertBegin(CodeFlag from, CodeFlag to);

    /**
     * @brief 编码转换
     * 
     * @param from 源字符串
     * @param to 目的字符串
     * @return true 
     * @return false 
     */
    bool convert(const std::string &from, std::string &to);

    /**
     * @brief 编码转换结束
     * 
     */
    void convertEnd();

    /**
     * @brief 将utf8转为gbk
     * 
     * @param u8String [in] utf8编码字符串
     * @param gbkString [out] 输出gbk编码
     * @return int32_t
     */
    static int32_t UTF8ToGBK(const std::string &u8String, std::string &gbkString);

    /**
     * @brief 将utf8转为gbk
     * 
     * @param gbkString
     * @param u8String 
     * @return int32_t
     */
    static int32_t GBKToUTF8(const std::string &gbkString, std::string &u8String);

    /**
     * @brief 将utf8转为gbk
     * 
     * @param u8String
     * @param u16String 
     * @return int32_t
     */
    static int32_t UTF8ToUTF16LE(const std::string &u8String, std::string &u16String);

    /**
     * @brief 将utf8转为gbk
     * 
     * @param u16String
     * @param u8String 
     * @return int32_t
     */
    static int32_t UTF16LEToUTF8(const std::string &u16String, std::string &u8String);

protected:
    const char *_flag2str(CodeFlag flag);
    static uint32_t _computeOutSize(CodeFlag from, CodeFlag to, uint32_t inputSize);
    static uint32_t _computeOutSizeToGBK_UTF16(CodeFlag from, CodeFlag to, uint32_t inputSize);
    static uint32_t _computeOutSizeToUTF8(CodeFlag from, CodeFlag to, uint32_t inputSize);

private:
    typedef void * iconv_t;

    iconv_t     m_codeConvHandle;
    uint32_t    m_cacheSize;
    CodeFlag    m_codeFrom;
    CodeFlag    m_codeTo;
};

} // namespace eular

#endif // __EULAR_CODE_CONVERT_H__
