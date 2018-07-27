#include "JwtToken.h"

using namespace az;

JwtToken::JwtToken(const utility::string_t& tokenType, const utility::string_t& token)
  : m_tokenType(tokenType), m_token(token)
{

}

JwtToken::~JwtToken()
{

}

utility::string_t JwtToken::value() const
{
  return m_token;
}

utility::string_t JwtToken::type() const
{
  return m_tokenType;
}

