#include "AuthEndpoint.h"

using namespace az;

AuthEndpoint::AuthEndpoint(const std::string& loginUri, const std::string& authUri)
  : m_loginUri(loginUri),
    m_authUri(authUri)
{

}

AuthEndpoint::~AuthEndpoint() {}

std::string AuthEndpoint::getLoginUri() const
{
  return m_loginUri;
}

std::string AuthEndpoint::getAuthUri() const
{
  return m_authUri;
}
