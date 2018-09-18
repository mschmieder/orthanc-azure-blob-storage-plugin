#include "JwtToken.h"

using namespace az;

JwtToken::JwtToken(const std::string& tokenType, const std::string& token)
  : m_tokenType(tokenType), m_token(token)
{

}

JwtToken::~JwtToken()
{

}

std::string JwtToken::value() const
{
  return m_token;
}

std::string JwtToken::type() const
{
  return m_tokenType;
}

