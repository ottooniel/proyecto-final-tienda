#ifndef CORE_FECHA_H
#define CORE_FECHA_H

#include <cctype>
#include <ctime>
#include <string>
#include "core.h"

// Fecha de hoy "AAAA-MM-DD"
string fechaActual()
{
    time_t ahora = time(nullptr);
    struct tm *local = localtime(&ahora);
    if (local == nullptr)
    {
        return "1970-01-01";
    }
    char texto[16];
    strftime(texto, sizeof(texto), "%Y-%m-%d", local);
    return string(texto);
}

// Hora actual "HH:MM"
string horaActual()
{
    time_t ahora = time(nullptr);
    struct tm *local = localtime(&ahora);
    if (local == nullptr)
    {
        return "00:00";
    }
    char texto[16];
    strftime(texto, sizeof(texto), "%H:%M", local);
    return string(texto);
}

// "AAAA-MM-DD" con mes de 1 a 12 y dia de 1 a 31
bool fechaValida(const string &texto)
{
    if (texto.length() != 10 || texto[4] != '-' || texto[7] != '-')
    {
        return false;
    }
    for (int i = 0; i < 10; i++)
    {
        if (i != 4 && i != 7 && !isdigit((unsigned char)texto[i]))
        {
            return false;
        }
    }
    int mes = (texto[5] - '0') * 10 + (texto[6] - '0');
    int dia = (texto[8] - '0') * 10 + (texto[9] - '0');
    return mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31;
}

// "HH:MM" con hora de 0 a 23 y minuto de 0 a 59
bool horaValida(const string &texto)
{
    if (texto.length() != 5 || texto[2] != ':')
    {
        return false;
    }
    for (int i = 0; i < 5; i++)
    {
        if (i != 2 && !isdigit((unsigned char)texto[i]))
        {
            return false;
        }
    }
    int hora = (texto[0] - '0') * 10 + (texto[1] - '0');
    int minuto = (texto[3] - '0') * 10 + (texto[4] - '0');
    return hora <= 23 && minuto <= 59;
}

#endif
