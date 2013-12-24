/* 
 * File:   utils.h
 * Author: brigada
 *
 * Created on 27 Май 2013 г., 21:23
 */

#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <vector>
#include <inttypes.h>

std::string stringf(std::string format, ...);
std::string implode(const std::string &delim, const std::vector<std::string> &items);
void DEBUG(std::string format, ...);
std::string TagTypeAsString(uint8_t type);
std::string string_escape(std::string str);
std::string string_replace(std::string search, std::string replace, std::string subject);

#endif	/* UTILS_H */

