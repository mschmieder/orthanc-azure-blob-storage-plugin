#include "AuthEndpoint.h"

using namespace az;

AuthEndpoint::AuthEndpoint(const utility::string_t& loginUri, const utility::string_t& authUri)
  : m_loginUri(loginUri),
    m_authUri(authUri)
{

}

AuthEndpoint::~AuthEndpoint() {}

utility::string_t AuthEndpoint::getLoginUri() const
{
  return m_loginUri;
}

utility::string_t AuthEndpoint::getAuthUri() const
{
  return m_authUri;
}
