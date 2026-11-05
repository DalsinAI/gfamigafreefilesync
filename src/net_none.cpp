// net_none.cpp - this port syncs folders the Amiga can reach as volumes
// (local drives, network shares mounted by the TCP/IP stack's file
// systems). FreeFileSync's own FTP, SFTP and Google Drive clients and its
// HTTP calls need libcurl, libssh2 and OpenSSL, which this build leaves
// out: afs/ftp.cpp, afs/sftp.cpp, afs/gdrive.cpp, afs/init_curl_libssh2.cpp,
// zen/http.cpp, zen/open_ssl.cpp and libcurl/curl_wrap.cpp are not built.
// Their headers stay, so the dialogs still compile; here an ftp://,
// sftp:// or gdrive:// path is never taken (the native file system gets
// it), and asking one of them for anything says it is not in this build.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include "afs/ftp.h"
#include "afs/sftp.h"
#include "afs/gdrive.h"
#include "afs/native.h"
#include <zen/http.h>

using namespace zen;
using namespace fff;

namespace
{
const wchar_t notInBuild[] = L"Not in this build: the Amiga build of FreeFileSync syncs folders on the Amiga's own volumes.";

[[noreturn]] void unavailable(const wchar_t* what)
{
    throw FileError(replaceCpy<std::wstring>(L"%x is not available.", L"%x", what), notInBuild);
}
}

// --- FTP ---------------------------------------------------------------------
bool fff::acceptsItemPathPhraseFtp(const Zstring& itemPathPhrase) { return false; }
AbstractPath fff::createItemPathFtp(const Zstring& itemPathPhrase) { return createItemPathNative(itemPathPhrase); }
void fff::ftpInit() {}
void fff::ftpTeardown() {}
AfsDevice fff::condenseToFtpDevice(const FtpLogin& login) { return getNullPath().afsDevice; }
FtpLogin fff::extractFtpLogin(const AfsDevice& afsDevice) { return {}; }
AfsPath fff::getFtpHomePath(const FtpLogin& login) { unavailable(L"FTP"); }

// --- SFTP --------------------------------------------------------------------
bool fff::acceptsItemPathPhraseSftp(const Zstring& itemPathPhrase) { return false; }
AbstractPath fff::createItemPathSftp(const Zstring& itemPathPhrase) { return createItemPathNative(itemPathPhrase); }
void fff::sftpInit() {}
void fff::sftpTeardown() {}
AfsDevice fff::condenseToSftpDevice(const SftpLogin& login) { return getNullPath().afsDevice; }
SftpLogin fff::extractSftpLogin(const AfsDevice& afsDevice) { return {}; }
int fff::getServerMaxChannelsPerConnection(const SftpLogin& login) { unavailable(L"SFTP"); }
AfsPath fff::getSftpHomePath(const SftpLogin& login) { unavailable(L"SFTP"); }

// --- Google Drive -------------------------------------------------------------
bool fff::acceptsItemPathPhraseGdrive(const Zstring& itemPathPhrase) { return false; }
AbstractPath fff::createItemPathGdrive(const Zstring& itemPathPhrase) { return createItemPathNative(itemPathPhrase); }
void fff::gdriveInit(const Zstring& configDirPath, const Zstring& caCertFilePath) {}
void fff::gdriveTeardown() {}
std::string fff::gdriveAddUser(const std::function<void()>& updateGui, int timeoutSec) { unavailable(L"Google Drive"); }
void fff::gdriveRemoveUser(const std::string& accountEmail, int timeoutSec) { unavailable(L"Google Drive"); }
std::vector<std::string> fff::gdriveListAccounts() { return {}; }
std::vector<Zstring> fff::gdriveListLocations(const std::string& accountEmail, int timeoutSec) { unavailable(L"Google Drive"); }
AfsDevice fff::condenseToGdriveDevice(const GdriveLogin& login) { return getNullPath().afsDevice; }
GdriveLogin fff::extractGdriveLogin(const AfsDevice& afsDevice) { return {}; }
Zstring fff::getGoogleDriveFolderUrl(const AbstractPath& folderPath) { unavailable(L"Google Drive"); }

// --- HTTP ----------------------------------------------------------------------
class zen::HttpInputStream::Impl {};
HttpInputStream::HttpInputStream(std::unique_ptr<Impl>&& pimpl) : pimpl_(std::move(pimpl)) {}
HttpInputStream::~HttpInputStream() {}
size_t HttpInputStream::tryRead(void* buffer, size_t bytesToRead) { return 0; }
size_t HttpInputStream::getBlockSize() const { return 64 * 1024; }
std::string HttpInputStream::readAll(const IoCallback& notifyUnbufferedIO) { return {}; }

HttpInputStream zen::sendHttpGet(const Zstring& url, const Zstring& userAgent, const Zstring& caCertFilePath)
{
    throw SysError(notInBuild);
}
HttpInputStream zen::sendHttpPost(const Zstring& url, const std::vector<std::pair<std::string, std::string>>& postParams,
                                  const IoCallback& notifyUnbufferedIO, const Zstring& userAgent, const Zstring& caCertFilePath)
{
    throw SysError(notInBuild);
}
HttpInputStream zen::sendHttpPost(const Zstring& url, const std::string& postBuf, const std::string& contentType,
                                  const IoCallback& notifyUnbufferedIO, const Zstring& userAgent, const Zstring& caCertFilePath)
{
    throw SysError(notInBuild);
}

bool zen::internetIsAlive() { return false; }

std::wstring zen::formatHttpError(int httpStatus)
{
    return L"HTTP status " + numberTo<std::wstring>(httpStatus);
}

bool zen::isValidEmail(const std::string_view& email)
{
    //local@domain.tld: one @, something either side, a dot in the domain, no spaces
    const size_t at = email.find('@');
    if (at == std::string_view::npos || at == 0 || email.find('@', at + 1) != std::string_view::npos)
        return false;
    const std::string_view domain = email.substr(at + 1);
    const size_t dot = domain.find('.');
    return dot != std::string_view::npos && dot > 0 && dot + 1 < domain.size() &&
           std::none_of(email.begin(), email.end(), [](char c) { return c == ' ' || c == '\t' || c == '<' || c == '>'; });
}

std::string zen::htmlSpecialChars(const std::string_view& str)
{
    std::string out;
    for (const char c : str)
        switch (c)
        {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            case '\'': out += "&#39;"; break;
            default: out += c;
        }
    return out;
}

namespace
{
std::string urlEncode(const std::string_view& s)
{
    static const char hex[] = "0123456789ABCDEF";
    std::string out;
    for (const unsigned char c : s)
        if (isAsciiAlpha(c) || isDigit(c) || c == '-' || c == '.' || c == '_' || c == '~')
            out += static_cast<char>(c);
        else if (c == ' ')
            out += '+';
        else
        {
            out += '%';
            out += hex[c >> 4];
            out += hex[c & 15];
        }
    return out;
}

std::string urlDecode(const std::string_view& s)
{
    auto val = [](char c) { return c >= 'a' ? c - 'a' + 10 : c >= 'A' ? c - 'A' + 10 : c - '0'; };
    std::string out;
    for (size_t i = 0; i < s.size(); ++i)
        if (s[i] == '+')
            out += ' ';
        else if (s[i] == '%' && i + 2 < s.size() && isHexDigit(s[i + 1]) && isHexDigit(s[i + 2]))
        {
            out += static_cast<char>(val(s[i + 1]) * 16 + val(s[i + 2]));
            i += 2;
        }
        else
            out += s[i];
    return out;
}
}

std::string zen::xWwwFormUrlEncode(const std::vector<std::pair<std::string, std::string>>& paramPairs)
{
    std::string out;
    for (const auto& [name, value] : paramPairs)
        out += (out.empty() ? "" : "&") + urlEncode(name) + '=' + urlEncode(value);
    return out;
}

std::vector<std::pair<std::string, std::string>> zen::xWwwFormUrlDecode(const std::string_view str)
{
    std::vector<std::pair<std::string, std::string>> out;
    for (const std::string_view nvPair : splitCpy(str, '&', SplitOnEmpty::skip))
        out.emplace_back(urlDecode(beforeFirst(nvPair, '=', IfNotFoundReturn::all)),
                         urlDecode(afterFirst (nvPair, '=', IfNotFoundReturn::none)));
    return out;
}
