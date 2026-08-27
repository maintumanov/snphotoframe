#include "qsnshapes.h"

bool QSNCelsius = true;
bool QSNMillimetersOfMercury = true;

// description of the types
QString QSNTypeLabel(quint8 index)
{
    switch (index) {
    case 0: return QString(QObject::tr(""));
    case 1: return QString(QObject::tr("BOOL"));
    case 2: return QString(QObject::tr("CO2"));
    case 3: return QString(QObject::tr("Air velocity"));
    case 4: return QString(QObject::tr("Water consumption"));
    case 5: return QString(QObject::tr("uInt8"));
    case 6: return QString(QObject::tr("Int8"));
    case 7: return QString(QObject::tr("uInt16"));
    case 8: return QString(QObject::tr("Int16"));
    case 9: return QString(QObject::tr("Temperature"));
    case 10: return QString(QObject::tr("Power"));
    case 11: return QString(QObject::tr("Current"));
    case 12: return QString(QObject::tr("Voltage"));
    case 13: return QString(QObject::tr("Date and time"));
    case 14: return QString(QObject::tr("Alert"));
    case 15: return QString(QObject::tr("Error"));
    case 16: return QString(QObject::tr("Text"));
    case 17: return QString(QObject::tr("Date"));
    case 18: return QString(QObject::tr("Time"));
    case 19: return QString(QObject::tr("Consumed electricity"));
    case 20: return QString(QObject::tr("Humidity"));
    case 21: return QString(QObject::tr("Pressure"));
    case 22: return QString(QObject::tr("Level"));
    case 23: return QString(QObject::tr("Distance"));
    case 24: return QString(QObject::tr("Angle"));
    case 25: return QString(QObject::tr("Illumination"));
    case 26: return QString(QObject::tr("uInt32"));
    case 27: return QString(QObject::tr("Int32"));
    case 28: return QString(QObject::tr("Capacity"));
    case 29: return QString(QObject::tr("Weight"));
    case 30: return QString(QObject::tr("Hex code"));
    case 31: return QString(QObject::tr("Security alarm"));
    case 32: return QString(QObject::tr("SNIR package"));
    case 33: return QString(QObject::tr("EM-Marine UID"));
    case 34: return QString(QObject::tr("Rainfall"));
    case 35: return QString(QObject::tr("Turnovers"));
    case 63: return QString(QObject::tr("Variant"));
    }
    return QString();
}

QString QSNTypeLatianName(quint8 index)
{
    switch (index) {
    case 0: return QString();
    case 1: return QLatin1String("bool");
    case 2: return QLatin1String("co2");
    case 3: return QLatin1String("airVelocity");
    case 4: return QLatin1String("waterConsumption");
    case 5: return QLatin1String("uint8");
    case 6: return QLatin1String("int8");
    case 7: return QLatin1String("uint16");
    case 8: return QLatin1String("int16");
    case 9: return QLatin1String("temp");
    case 10: return QLatin1String("power");
    case 11: return QLatin1String("current");
    case 12: return QLatin1String("voltage");
    case 13: return QLatin1String("datetime");
    case 14: return QLatin1String("alert");
    case 15: return QLatin1String("error");
    case 16: return QLatin1String("text");
    case 17: return QLatin1String("date");
    case 18: return QLatin1String("time");
    case 19: return QLatin1String("energy");
    case 20: return QLatin1String("humidity");
    case 21: return QLatin1String("pressure");
    case 22: return QLatin1String("level");
    case 23: return QLatin1String("distance");
    case 24: return QLatin1String("angle");
    case 25: return QLatin1String("illumination");
    case 26: return QLatin1String("uint32");
    case 27: return QLatin1String("int32");
    case 28: return QLatin1String("capacity");
    case 29: return QLatin1String("weight");
    case 30: return QLatin1String("hex");
    case 31: return QLatin1String("security");
    case 32: return QLatin1String("snir");
    case 33: return QLatin1String("emUID");
    case 34: return QLatin1String("rainfall");
    case 35: return QLatin1String("turnovers");
    case 63: return QLatin1String("variant");
    }
    return QString();
}


int QSNTypeIconIndex(quint8 index)
{
    switch (index) {
    case 0: return 1;
    case 1: return 76;
        //    case 2: return 2;
        //    case 3: return 77;
        //    case 4: return 76;
    case 5: return 78;
    case 6: return 78;
    case 7: return 78;
    case 8: return 78;
    case 9: return 22;
    case 10: return 12;
    case 11: return 79;
    case 12: return 80;
    case 13: return 81;
    case 14: return 82;
    case 15: return 83;
    case 16: return 99;
    case 17: return 36;
    case 18: return 35;
    case 19: return 12;
    case 20: return 85;
    case 21: return 86;
    case 22: return 87;
    case 23: return 88;
    case 24: return 89;
    case 25: return 90;
    case 26: return 78;
    case 27: return 78;
    case 28: return 91;
    case 29: return 92;
    case 30: return 93;
    case 31: return 23;
    case 32: return 97;
    case 33: return 98;
    case 63: return 78;
    }
    return 1;
}

QString QSNTypeNote(quint8 index)
{
    switch (index) {
    case 0: return QString(QObject::tr("None"));
    case 1: return QString(QObject::tr("TRUE/FALSE"));
    case 2: return QString(QObject::tr("CO2 0...65535ppm"));
    case 3: return QString(QObject::tr("Air velocity 0...65535dm/sec"));
    case 4: return QString(QObject::tr("Water consumption 0...65535dm3"));
    case 5: return QString(QObject::tr("uInt8 0...255"));
    case 6: return QString(QObject::tr("Int8 -127...127"));
    case 7: return QString(QObject::tr("uInt16 0...65535"));
    case 8: return QString(QObject::tr("Int16 -32767…32767"));
    case 9: return QString(QObject::tr("Temperature -127.99..+128 C"));
    case 10: return QString(QObject::tr("Power -2147483647…+2147483647 mW"));
    case 11: return QString(QObject::tr("Current -2147483647…+2147483647 mA"));
    case 12: return QString(QObject::tr("Voltage -2147483647…+2147483647 mV"));
    case 13: return QString(QObject::tr("Date and time"));
    case 14: return QString(QObject::tr("Alert"));
    case 15: return QString(QObject::tr("Error"));
    case 16: return QString(QObject::tr("Text"));
    case 17: return QString(QObject::tr("Date"));
    case 18: return QString(QObject::tr("Time"));
    case 19: return QString(QObject::tr("Consumed electricity"));
    case 20: return QString(QObject::tr("Humidity 0..100%"));
    case 21: return QString(QObject::tr("Pressure 0...500000 Pa"));
    case 22: return QString(QObject::tr("Level 0..100%"));
    case 23: return QString(QObject::tr("Distance 0...65535 mm"));
    case 24: return QString(QObject::tr("Angle -21600…21600 minutus"));
    case 25: return QString(QObject::tr("Lux 0...65535 lx"));
    case 26: return QString(QObject::tr("uInt32 0...4294967296"));
    case 27: return QString(QObject::tr("Int32 -2147483647…+2147483647"));
    case 28: return QString(QObject::tr("Capacity 0...4294967296 ml"));
    case 29: return QString(QObject::tr("Weight 0...4294967296 mg"));
    case 30: return QString(QObject::tr("Hex code 32 bit"));
    case 31: return QString(QObject::tr("Security alarm"));
    case 32: return QString(QObject::tr("SNIR package"));
    case 33: return QString(QObject::tr("EM-Marine UID"));
    case 34: return QString(QObject::tr("Rainfall 0...65535mm"));
    case 35: return QString(QObject::tr("Turnovers 0...100000rpm"));
    case 63: return QString(QObject::tr("Variant (Any)"));
    }
    return QString();
}

QString QSNTypePfAlt(int multiplier, QString h, QString n, QString l)
{
    switch (multiplier) {
    case 1: return h;
    case -1: return l;
    default: return n;
    }
}

QString QSNScalingNumToString(quint8 index, qreal num)
{
    switch (index) {
    case 3:
        if (num >= 10) return QString("%1%2").arg(num / 10, 0, 'f', 1).arg(QSNTypePostFix(index, 1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 4:
        if (num >= 10) return QString("%1%2").arg(num / 10, 0, 'f', 1).arg(QSNTypePostFix(index, 1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 10:
        if (num >= 1000000) return QString("%1%2").arg(num / 1000000, 0, 'f', 1).arg(QSNTypePostFix(index, 1));
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 1).arg(QSNTypePostFix(index, -1));
        return QString("%1%2").arg(num, 0, 'f', -1).arg(QSNTypePostFix(index, 0));
    case 11:
        if (num >= 1000000) return QString("%1%2").arg(num / 1000000, 0, 'f', 1).arg(QSNTypePostFix(index, 1));
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 1).arg(QSNTypePostFix(index, -1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 12:
        if (num >= 1000000) return QString("%1%2").arg(num / 1000000, 0, 'f', 1).arg(QSNTypePostFix(index, 1));
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 1).arg(QSNTypePostFix(index, -1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 19:
        if (num >= 1000000) return QString("%1%2").arg(num / 1000000, 0, 'f', 1).arg(QSNTypePostFix(index, 1));
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 1).arg(QSNTypePostFix(index, -1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 23:
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 2).arg(QSNTypePostFix(index, 1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 26:
        return QString("%1%2").arg(num).arg(QSNTypePostFix(index, 0));
    case 27:
        return QString("%1 %2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 28:
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 1).arg(QSNTypePostFix(index, 0));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, -1));
    case 29:
        if (num >= 1000000) return QString("%1%2").arg(num / 1000000, 0, 'f', 2).arg(QSNTypePostFix(index, 1));
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 2).arg(QSNTypePostFix(index, -1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    case 34:
        if (num >= 1000) return QString("%1%2").arg(num / 1000, 0, 'f', 3).arg(QSNTypePostFix(index, 1));
        return QString("%1%2").arg(num, 0, 'f', 0).arg(QSNTypePostFix(index, 0));
    }
    return QString("%1%2").arg(num, 0, 'f', 3).arg(QSNTypePostFix(index));
}

QString QSNTypePostFix(quint8 index, int multiplier)
{
    if (index == 2) return QSNTypePfAlt(multiplier, QObject::tr("ppm"), QObject::tr("ppm"), QObject::tr("ppm"));
    if (index == 3) return QSNTypePfAlt(multiplier, QObject::tr("m/sec"), QObject::tr("dm/sec"), QObject::tr("dm/sec"));
    if (index == 4) return QSNTypePfAlt(multiplier, QObject::tr("m3"), QObject::tr("dm3"), QObject::tr("dm3"));
    if (index == 9 && QSNCelsius) return QObject::tr("°C");
    if (index == 9 && !QSNCelsius) return QObject::tr("F");
    if (index == 10) return QSNTypePfAlt(multiplier, QObject::tr("kWt"), QObject::tr("mWt"), QObject::tr("Wt"));
    if (index == 11) return QSNTypePfAlt(multiplier, QObject::tr("kA"), QObject::tr("mA"), QObject::tr("A"));
    if (index == 12) return QSNTypePfAlt(multiplier, QObject::tr("kV"), QObject::tr("mV"), QObject::tr("V"));
    if (index == 19) return QSNTypePfAlt(multiplier, QObject::tr("kW.h"), QObject::tr("mW.h"), QObject::tr("W.h"));
    if (index == 20) return QObject::tr("%");
    if (index == 21 && QSNMillimetersOfMercury) return QObject::tr("mmHg");
    if (index == 21 && !QSNMillimetersOfMercury) return QObject::tr("hPa");
    if (index == 22) return QObject::tr("%");
    if (index == 23) return QSNTypePfAlt(multiplier, QObject::tr("m"), QObject::tr("mm"), QObject::tr("mkm"));
    if (index == 24) return QSNTypePfAlt(multiplier, QObject::tr("hours"), QObject::tr("minutus"), QObject::tr("seconds"));
    if (index == 25) return QSNTypePfAlt(multiplier, QObject::tr("klx"), QObject::tr("lx"), QObject::tr("mlx"));
    if (index == 28) return QSNTypePfAlt(multiplier, QObject::tr("l"), QObject::tr("l"), QObject::tr("ml"));
    if (index == 29) return QSNTypePfAlt(multiplier, QObject::tr("kg"), QObject::tr("mg"), QObject::tr("g"));
    if (index == 34) return QSNTypePfAlt(multiplier, QObject::tr("m"), QObject::tr("mm"), QObject::tr("mm"));
    if (index == 35) return QObject::tr("rpm");
    return QString();
}

QString QSNTypePreFix(quint8 index)
{
    if (index == 1) return QObject::tr("State");
    if (index == 2) return QString(QObject::tr("CO2"));
    if (index == 3) return QString(QObject::tr("Air velocity"));
    if (index == 4) return QString(QObject::tr("Water consumption"));
    if (index > 4 && index < 9) return QObject::tr("Number");
    if (index == 9) return QObject::tr("Temperature");
    if (index == 10) return QObject::tr("Power");
    if (index == 11) return QObject::tr("Current");
    if (index == 12) return QObject::tr("Voltage");
    if (index == 13) return QObject::tr("Time");
    if (index == 14) return QObject::tr("Alert");
    if (index == 15) return QObject::tr("Error");
    if (index == 16) return QObject::tr("Text");
    if (index == 17) return QObject::tr("Date");
    if (index == 18) return QObject::tr("Time");
    if (index == 19) return QObject::tr("Electricity");
    if (index == 20) return QObject::tr("Humidity");
    if (index == 21) return QObject::tr("Pressure");
    if (index == 22) return QObject::tr("Level");
    if (index == 23) return QObject::tr("Distance");
    if (index == 24) return QObject::tr("Angle");
    if (index == 25) return QObject::tr("Illumination");
    if (index == 26) return QObject::tr("Number");
    if (index == 27) return QObject::tr("Number");
    if (index == 28) return QObject::tr("Capacity");
    if (index == 29) return QObject::tr("Weight");
    if (index == 30) return QObject::tr("Code");
    if (index == 31) return QObject::tr("Security alarm");
    if (index == 32) return QObject::tr("SNIR");
    if (index == 33) return QObject::tr("UID");
    if (index == 34) return QLatin1String("Rainfall");
    if (index == 35) return QLatin1String("Turnovers");
    if (index == 63) return QObject::tr("Variant");
    return QString();
}

int QSNTypeDataSize(quint8 index)
{
    if (index == 1) return 1;
    if (index > 1 && index < 5) return 2;
    if (index > 4 && index < 7) return 1;
    if (index > 6 && index < 10) return 2;
    if (index > 9 && index < 13) return 4;
    if (index == 13) return 7;
    if (index == 14) return 3;
    if (index > 16 && index < 19) return 4;
    if (index == 19) return 5;
    if (index == 20) return 1;
    if (index == 21) return 4;
    if (index == 22) return 1;
    if (index > 22 && index <= 25) return 2;
    if (index > 25 && index < 31) return 4;
    if (index == 31) return 3;
    if (index == 32) return 8;
    if (index == 33) return 4;
    if (index == 34) return 2;
    if (index == 35) return 4;
    return 0;
}

QPointF QSNTypeRange(quint8 index)
{
    switch (index) {
    case 1: return QPointF(0, 1);
    case 2: return QPointF(0, 65535);
    case 3: return QPointF(0, 65535);
    case 4: return QPointF(0, 65535);
    case 5: return QPointF(0, 255);
    case 6: return QPointF(-127, 127);
    case 7: return QPointF(0, 65535);
    case 8: return QPointF(-32767, 32767);
    case 9: return QPointF(-127.99, 128);
    case 10: return QPointF(-2147483647, 2147483647);
    case 11: return QPointF(-2147483647, 2147483647);
    case 12: return QPointF(-2147483647, 2147483647);
        //  case 13: return QPointF(0, 255);
        //   case 14: return QPointF(0, 255);
        //   case 15: return QPointF(0, 255);
        //   case 16: return QPointF(0, 255);
        //   case 17: return QPointF(0, 255);
        //   case 18: return QPointF(0, 255);
    case 19: return QPointF(0, 65535);
    case 20: return QPointF(0, 100);
    case 21: return QPointF(0, 4294967296);
    case 22: return QPointF(0, 100);
    case 23: return QPointF(0, 65535);
    case 24: return QPointF(-21600, 21600);
    case 25: return QPointF(0, 65535);
    case 26: return QPointF(0, 4294967296);
    case 27: return QPointF(-2147483647, 2147483647);
    case 28: return QPointF(0, 4294967296);
    case 29: return QPointF(0, 4294967296);
    case 30: return QPointF(0, 4294967296);
    case 33: return QPointF(0, 4294967296);
    case 34: return QPointF(0, 65535);
    case 35: return QPointF(0, 100000);
    }
    return QPointF(0, 1);
}

int QSNAutoIcon(int iconNum, int dataType)
{
    if (iconNum > 0) return iconNum;
    return QSNTypeIconIndex(static_cast<quint8>(dataType));
}

bool QSNTypeCompatibility(quint8 type1, bool input1, quint8 type2, bool input2)
{
    if (type1 == type2) return true;
    if ((type1 == 30 && type2 == 26) || (type1 == 26 && type2 == 30)) return true;

    if (input1) {
        if (type1 == 63) return true;

    }

    if (input2) {
        if (type2 == 63) return true;

    }
    return false;
}

QString QSNTypeRAWtoString(QByteArray *Data, QString text)
{
    if (Data->count() == 0) Data->append(static_cast<char>(0));
    quint8 t = static_cast<quint8>(Data->at(0));
    if (t == 0) return QString();
    if (t == 1) {
        if (Data->size() > 2) return QSNBoolTypetoLabel(QSNRAWtoBool(Data, 1), static_cast<quint8>(Data->at(2)));
        else return QSNBoolTypetoLabel(QSNRAWtoBool(Data, 1), 0);
    }
    if (t == 2) return QString("%1%2").arg(QSNRAWtoUInt16(Data, 1)).arg(QSNTypePostFix(2));
    if (t == 3) return QString("%1%2").arg(QSNRAWtoUInt16(Data, 1)).arg(QSNTypePostFix(3));
    if (t == 4) return QString("%1%2").arg(QSNRAWtoUInt16(Data, 1)).arg(QSNTypePostFix(4));
    if (t == 5) return QString("%1%2").arg(QSNRAWtoByte(Data, 1)).arg(QSNRAWtoPostFix(Data, 2));
    if (t == 6) return QString("%1%2").arg(QSNRAWtoInt8(Data, 1)).arg(QSNRAWtoPostFix(Data, 2));
    if (t == 7) return QString("%1%2").arg(QSNRAWtoUInt16(Data, 1)).arg(QSNRAWtoPostFix(Data, 3));
    if (t == 8) return QString("%1%2").arg(QSNRAWtoInt16(Data, 1)).arg(QSNRAWtoPostFix(Data, 3));
    if (t == 9) return QString("%1%2").arg(QSNRAWtoTemperature(Data, 1), 0, 'f', 1).arg(QSNTypePostFix(9));
    if (t == 10) return QSNScalingNumToString(10, static_cast<qreal>(QSNRAWtoInt32(Data, 1)));
    if (t == 11) return QSNScalingNumToString(11, static_cast<qreal>(QSNRAWtoInt32(Data, 1)));
    if (t == 12) return QSNScalingNumToString(12, static_cast<qreal>(QSNRAWtoInt32(Data, 1)));
    if (t == 13) return QString("%1").arg(QSNRAWtoDateTime(Data, 1).toString(QLatin1String("hh:mm:ss-dd.MM.yyyy")));
    if (t == 14) {
        QSNNotification notif = QSNRAWtoNotification(Data, 1);
        return QString("%1").arg(QSNAlertTypeLabel(notif.notificationType));
    }
    if (t == 15) {
        QSNError error = QSNRAWtoError(Data, 1);
        return QObject::tr("%1, device address: %2").arg(error.errorType, 0, 16).arg(error.deviceAddress);
    }
    if (t == 16) return QSNRAWtoString(Data, text, 1);
    if (t == 17) return QString("%1").arg(QSNRAWtoDate(Data, 1).toString(QLatin1String("dd.MM.yyyy")));
    if (t == 18) return QString("%1").arg(QSNRAWtoTime(Data, 1).toString(QLatin1String("hh:mm:ss")));
    if (t == 19) {
        QSNPower pw = QSNRAWtoPower(Data, 1);
        return QSNScalingNumToString(19, static_cast<double>(pw.power) * pw.seconds / 3600);

    }
    if (t == 20) return QString("%1%2").arg(QSNRAWtoByte(Data, 1)).arg(QSNTypePostFix(20));
    if (t == 21) return QString("%1%2").arg(QSNToMillimetersOfMercury(QSNRAWtoUInt32(Data, 1)), 0, 'f', 2).arg(QSNTypePostFix(21));
    if (t == 22) return QString("%1%2").arg(QSNRAWtoByte(Data, 1)).arg(QSNTypePostFix(22));
    if (t == 23) return QSNScalingNumToString(23, static_cast<qreal>(QSNRAWtoUInt16(Data, 1)));
    if (t == 24) return QSNAngleToString(QSNRAWtoInt16(Data, 1));
    if (t == 25) return QString("%1%2").arg(QSNRAWtoUInt16(Data, 1)).arg(QSNTypePostFix(25));
    if (t == 26) return QString("%1%2").arg(QSNRAWtoUInt32(Data, 1)).arg(QSNTypePostFix(26));
    if (t == 27) return QString("%1%2").arg(QSNRAWtoInt32(Data, 1)).arg(QSNTypePostFix(27));
    if (t == 28) return QSNScalingNumToString(28, static_cast<qreal>(QSNRAWtoUInt32(Data, 1)));
    if (t == 29) return QSNScalingNumToString(29, static_cast<qreal>(QSNRAWtoUInt32(Data, 1)));
    if (t == 30) return QString("%1").arg(QSNRAWtoHEXcode(Data, 1));
    if (t == 31) {
        QSNNotification notif = QSNRAWtoNotification(Data, 1);
        return QString("%1").arg(QSNSecurityAlertTypeLabel(notif.notificationType));
    }
    if (t == 32) {
        QSNSNIR snir = QSNRAWtoSNIR(Data, 1);
        return QString("A%1/P%2 (%3)").arg(snir.address).arg(snir.parameter).arg(QSNTypeRAWtoLabel(&snir.Data));
    }
    if (t == 33) {
        QSNEMUID uid = QSNRAWtoEmUID(Data, 1);
        return QString("%1/%2").arg(uid.family).arg(uid.number);
    }
    if (t == 34) return QSNScalingNumToString(34, static_cast<qreal>(QSNRAWtoUInt16(Data, 1)));
    if (t ==35) return QString("%1%2").arg(QSNRAWtoUInt32(Data, 1)).arg(QSNTypePostFix(35));
    return QString();
}

quint8 QSNRAWtoPostFixIndex(QByteArray *data, quint8 index)
{
    if (data->size() <= index) return 0;
    return static_cast<quint8>(data->at(index));
}

QString QSNRAWtoPostFix(QByteArray *data, quint8 index)
{
    return QSNTypePostFix(QSNRAWtoPostFixIndex(data, index));
}


QString QSNRAWtoNotice(QByteArray *data)
{
    if (data->size() < 3) return QString();
    if (data->at(0) == 1) {
        if (static_cast<quint8>(data->at(2)) < 100) return QString();
        return QSNBoolTypetoLabel(false, static_cast<quint8>(data->at(2)));
    }
    return QString();
}

QString QSNBoolTypetoLabel(bool state, quint8 index)
{
    switch (index) {
    case 100: return QString(QObject::tr("intermediate state"));
    case 101: return QString(QObject::tr("switch"));
    case 102: return QString(QObject::tr("in process"));
    case 103: return QString(QObject::tr("closes"));
    case 104: return QString(QObject::tr("opens"));
    case 105: return QString(QObject::tr("runs"));
    case 106: return QString(QObject::tr("stops"));
    case 107: return QString(QObject::tr("absents"));
    case 200: return QString(QObject::tr("error"));
    case 201: return QString(QObject::tr("accident"));
    case 202: return QString(QObject::tr("failure"));
    };

    if (state)
        switch (index) {
        case 1: return QString(QObject::tr("on"));
        case 2: return QString(QObject::tr("yes"));
        case 3: return QString(QObject::tr("open"));
        case 4: return QString(QObject::tr("elevated"));
        case 5: return QString(QObject::tr("open"));
        case 6: return QString(QObject::tr("allowed"));
        case 7: return QString(QObject::tr("presents"));
        case 8: return QString(QObject::tr("detected"));
        case 9: return QString(QObject::tr("activated"));
        default: return QString(QObject::tr("true"));
        }
    else
        switch (index) {
        case 1: return QString(QObject::tr("off"));
        case 2: return QString(QObject::tr("no"));
        case 3: return QString(QObject::tr("close"));
        case 4: return QString(QObject::tr("omitted"));
        case 5: return QString(QObject::tr("close"));
        case 6: return QString(QObject::tr("prohibited"));
        case 7: return QString(QObject::tr("absents"));
        case 8: return QString(QObject::tr("not detected"));
        case 9: return QString(QObject::tr("not activated"));
        default: return QString(QObject::tr("false"));
        }
}

QString QSNTypeRAWtoLabel(QByteArray *Data)
{
    if (Data->count() == 0) Data->append(static_cast<char>(0));
    QString ret =  QSNTypePreFix(static_cast<quint8>(Data->at(0)));
    if (!ret.isEmpty()) ret = ret + QLatin1String(": ");
    ret = ret + QSNTypeRAWtoString(Data);
    return ret;
}

qreal QSNRAWtoReal(QByteArray *data, quint8 index, int addr)
{
    QSNPower pw;
    switch (index)
    {
    case 1: return QSNRAWtoBool(data, addr);
    case 2: return QSNRAWtoUInt16(data, addr);
    case 3: return QSNRAWtoUInt16(data, addr);
    case 4: return QSNRAWtoUInt16(data, addr);
    case 5: return QSNRAWtoByte(data, addr);
    case 6: return QSNRAWtoInt8(data, addr);
    case 7: return QSNRAWtoUInt16(data, addr);
    case 8: return QSNRAWtoInt16(data, addr);
    case 9: return QSNRAWtoTemperature(data, addr);
    case 10: return QSNRAWtoInt32(data, addr);
    case 11: return QSNRAWtoInt32(data, addr);
    case 12: return QSNRAWtoInt32(data, addr);
    case 19: pw = QSNRAWtoPower(data, addr);
        return (static_cast<double>(pw.power) / 3600000 * pw.seconds);
    case 20: return QSNRAWtoByte(data, addr);
    case 21: return QSNToMillimetersOfMercury(QSNRAWtoUInt32(data, addr));
    case 22: return QSNRAWtoByte(data, addr);
    case 23: return QSNRAWtoUInt16(data, addr);
    case 24: return QSNRAWtoInt16(data, addr) * 3600;
    case 25: return QSNRAWtoUInt16(data, addr);
    case 26: return QSNRAWtoUInt32(data, addr);
    case 27: return QSNRAWtoInt32(data, addr);
    case 28: return QSNRAWtoUInt32(data, addr) * 1000;
    case 29: return QSNRAWtoUInt32(data, addr) * 1000;
    case 33: return QSNRAWtoUInt32(data, addr);
    case 34: return QSNRAWtoUInt16(data, addr);
    case 35: return QSNRAWtoUInt32(data, addr);
    }
    return 0;
}

quint16 QSNTypeRAWtoIDP(QByteArray *data)
{
    if (data->size() < 1) return 0;
    int s = QSNTypeDataSize(static_cast<quint8>(data->at(0)));
    if (data->size() < s + 3) return 0;
    return QSNRAWtoUInt16(data, s + 1);
}


quint16 QSNIDPtoDevAddr(quint16 idp)
{
    return (idp & 0x0FFF);
}

qreal QSNCelsiusToFahrenheit(qreal celsius)
{
    return 1.8 * celsius + 32;
}

qreal QSNFahrenheitToCelsius(qreal fahrenheit)
{
    return (fahrenheit - 32) / 1.8;
}

qreal QSNFromCelsius(qreal celsius)
{
    if (QSNCelsius) return celsius;
    return QSNCelsiusToFahrenheit(celsius);
}

qreal QSNToCelsius(qreal value)
{
    if (QSNCelsius) return value;
    return QSNFahrenheitToCelsius(value);
}

qreal QSNToMillimetersOfMercury(quint32 value)
{
    if (!QSNMillimetersOfMercury) return static_cast<qreal>(value / 100);
    return static_cast<qreal>(value * 0.0075);
}

QStringList QSNAlertTypeList()
{
    QStringList ret;
    quint8 index = 0;
    while (QSNAlertTypeLabel(index) != QString()) {
        ret.append(QSNAlertTypeLabel(index));
        index ++;
    }
    return ret;
}

QString QSNAlertTypeLabel(quint8 index)
{
    switch (index) {
    case 0: return QString(QObject::tr("Canceling alert"));
    case 1: return QString(QObject::tr("Fire"));
    case 2: return QString(QObject::tr("Stopped fire"));
    case 3: return QString(QObject::tr("Water leakage"));
    case 4: return QString(QObject::tr("Stopped water leakage"));
    case 5: return QString(QObject::tr("Is absent electricity supply"));
    case 6: return QString(QObject::tr("Electricity supply restored"));
    case 7: return QString(QObject::tr("Low oxygen levels"));
    case 8: return QString(QObject::tr("Oxygen levels are normal"));
    case 9: return QString(QObject::tr("Gas leak"));
    case 10: return QString(QObject::tr("Gas leak is absent"));
    }
    return QString();
}

QString QSNSecurityAlertTypeLabel(quint8 index)
{
    switch (index) {
    case 0: return QString(QObject::tr("Digit 0"));
    case 1: return QString(QObject::tr("Digit 1"));
    case 2: return QString(QObject::tr("Digit 2"));
    case 3: return QString(QObject::tr("Digit 3"));
    case 4: return QString(QObject::tr("Digit 4"));
    case 5: return QString(QObject::tr("Digit 5"));
    case 6: return QString(QObject::tr("Digit 6"));
    case 7: return QString(QObject::tr("Digit 7"));
    case 8: return QString(QObject::tr("Digit 8"));
    case 9: return QString(QObject::tr("Digit 9"));
    case 10: return QString(QObject::tr("Arming"));
    case 11: return QString(QObject::tr("Disarming"));
    case 12: return QString(QObject::tr("Penetration"));
    case 13: return QString(QObject::tr("Cancel the alarm"));
    case 14: return QString(QObject::tr("Arming (secretly)"));
    }
    return QString();
}

//==============================

bool QSNRAWtoBit(QByteArray *Data, int addr, int bit)
{
    if (Data->count() <= addr) return false;
    quint8 vol = static_cast<quint8>(Data->at(addr));
    vol >>= bit;
    vol &= 1;
    return (vol != 0);
}

void QSNBitToRAW(QByteArray *Data, int addr, int bit, bool state)
{
    if (Data->count() <= addr) Data->resize(addr + 1);
    quint8 msk = 1;
    msk <<= bit;
    quint8 vol = static_cast<quint8>(Data->at(addr));
    if (state) vol |= msk;
    else { msk = ~msk; vol &= msk; }
    (*Data)[addr] = static_cast<char>(vol);
}

bool QSNRAWtoBool(QByteArray *Data, int addr)
{
    if (Data->count() <= addr) return false;
    return (static_cast<quint8>(Data->at(addr)) > 0);
}

void QSNBoolToRAW(QByteArray *Data, int addr, bool state)
{
    if (Data->count() <= addr) Data->resize(addr + 1);
    if (state) (*Data)[addr] = static_cast<quint8>(1);
    else (*Data)[addr] = static_cast<quint8>(0);
}

quint8 QSNRAWtoByte(QByteArray *Data, int addr)
{
    if (Data->count() <= addr) return 0;
    return static_cast<quint8>(Data->at(addr));
}

void QSNByteToRAW(QByteArray *Data, int addr, quint8 value)
{
    if (Data->count() <= addr) Data->resize(addr + 1);
    (*Data)[addr] = static_cast<char>(value);
}

qint8 QSNRAWtoInt8(QByteArray *Data, int addr)
{
    if (Data->count() <= addr) return 0;
    union t_u {qint8 vol; quint8 D;};
    t_u t;
    t.D = static_cast<quint8>(Data->at(addr));
    return t.vol;
}

void QSNInt8ToRAW(QByteArray *Data, int addr, qint8 value)
{
    if (Data->count() <= addr) Data->resize(addr + 1);
    union t_u {qint8 vol; quint8 D;};
    t_u t;
    t.vol = value;
    (*Data)[addr] = static_cast<char>(t.D);
}

quint16 QSNRAWtoUInt16(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 1) return 0;
    union t_u {quint16 vol; quint8 D[2];};
    t_u t;
    t.D[0] = static_cast<quint8>(Data->at(addr + 1));
    t.D[1] = static_cast<quint8>(Data->at(addr));
    return t.vol;
}

void QSNUInt16ToRAW(QByteArray *Data, int addr, quint16 value)
{
    if (Data->count() <= addr + 1) Data->resize(addr + 2);
    union t_u {quint16 vol; quint8 D[2];};
    t_u t;
    t.vol = value;
    (*Data)[addr + 1] = static_cast<char>(t.D[0]);
    (*Data)[addr] = static_cast<char>(t.D[1]);
}

qint16 QSNRAWtoInt16(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 1) return 0;
    union t_u {qint16 vol; quint8 D[2];};
    t_u t;
    t.D[0] = static_cast<quint8>(Data->at(addr + 1));
    t.D[1] = static_cast<quint8>(Data->at(addr));
    return t.vol;
}

void QSNInt16ToRAW(QByteArray *Data, int addr, qint16 value)
{
    if (Data->count() <= addr + 1) Data->resize(addr + 2);
    union t_u {qint16 vol; quint8 D[2];};
    t_u t;
    t.vol = value;
    (*Data)[addr + 1] = static_cast<char>(t.D[0]);
    (*Data)[addr] = static_cast<char>(t.D[1]);
}

qint32 QSNRAWtoInt32(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 3) return 0;
    union t_u {qint32 vol; quint8 D[4];};
    t_u t;
    for (int i = 0; i < 4; i ++)
        t.D[i] = static_cast<quint8>(Data->at(addr + 3 - i));
    return t.vol;
}

void QSNInt32ToRAW(QByteArray *Data, int addr, qint32 value)
{
    if (Data->count() <= addr + 3) Data->resize(addr + 4);
    union t_u {qint32 vol; quint8 D[4];};
    t_u t;
    t.vol = value;
    for (int i = 0; i < 4; i ++)
        (*Data)[addr + 3 - i] = static_cast<char>(t.D[i]);
}

qint32 QSNRAWtoInt24(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 2) return 0;
    union t_u {qint32 vol; quint8 D[4];};
    t_u t;
    if (Data->at(addr) & 128) t.D[3] = 255;
    else t.D[3] = 0;
    for (int i = 0; i < 3; i ++)
        t.D[i] = static_cast<quint8>(Data->at(addr + 2 - i));
    return t.vol;
}

void QSNInt24ToRAW(QByteArray *Data, int addr, qint32 value)
{
    if (Data->count() <= addr + 2) Data->resize(addr + 3);
    union t_u {qint32 vol; quint8 D[4];};
    t_u t;
    t.vol = value;
    for (int i = 0; i < 3; i ++)
        (*Data)[addr + 2 - i] = static_cast<char>(t.D[i]);
}

qint64 QSNRAWtoInt64(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 7) return 0;
    union t_u {qint64 vol; quint8 D[8];};
    t_u t;
    for (int i = 0; i < 8; i ++)
        t.D[i] = static_cast<quint8>(Data->at(addr + 7 - i));
    return t.vol;
}

void QSNInt64ToRAW(QByteArray *Data, int addr, qint64 value)
{
    if (Data->count() <= addr + 7) Data->resize(addr + 8);
    union t_u {qint64 vol; quint8 D[8];};
    t_u t;
    t.vol = value;
    for (int i = 0; i < 8; i ++)
        (*Data)[addr + 7 - i] = static_cast<char>(t.D[i]);
}

quint32 QSNRAWtoUInt32(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 3) return 0;
    union t_u {quint32 vol; quint8 D[4];};
    t_u t;
    for (int i = 0; i < 4; i ++)
        t.D[i] = static_cast<quint8>(Data->at(addr + 3 - i));
    return t.vol;
}

void QSNUInt32ToRAW(QByteArray *Data, int addr, quint32 value)
{
    if (Data->count() <= addr + 3) Data->resize(addr + 4);
    union t_u {quint32 vol; quint8 D[4];};
    t_u t;
    t.vol = value;
    for (int i = 0; i < 4; i ++)
        (*Data)[addr + 3 - i] = static_cast<char>(t.D[i]);
}

quint64 QSNRAWtoUInt64(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 7) return 0;
    union t_u {quint64 vol; quint8 D[8];};
    t_u t;
    for (int i = 0; i < 8; i ++)
        t.D[i] = static_cast<quint8>(Data->at(addr + 7 - i));
    return t.vol;
}

void QSNUInt64ToRAW(QByteArray *Data, int addr, quint64 value)
{
    if (Data->count() <= addr + 7) Data->resize(addr + 8);
    union t_u {quint64 vol; quint8 D[8];};
    t_u t;
    t.vol = value;
    for (int i = 0; i < 8; i ++)
        (*Data)[addr + 7 - i] = static_cast<char>(t.D[i]);
}

qreal QSNRAWtoTemperature(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 1) return 0;
    quint16 vl = static_cast<quint8>(Data->at(addr));
    qreal temp;
    vl = static_cast<quint16>((vl << 8));
    vl += static_cast<quint8>(Data->at(addr + 1));
    if ((vl >> 15) == 0) {
        temp = (vl * 0.00390625);
    } else {
        vl = 0xFFFF^vl;
        vl ++;
        temp = (vl * -0.00390625);
    }
    if (QSNCelsius) return temp;
    else return QSNCelsiusToFahrenheit(temp);
}

void QSNTemperatureToRAW(QByteArray *Data, int addr, qreal value)
{
    if (Data->count() <= addr + 1) Data->resize(addr + 2);
    quint16 cv;
    qreal temp;
    if (QSNCelsius) temp = value;
    else temp = QSNFahrenheitToCelsius(value);
    if (value < 0) {
        cv = static_cast<quint16>(temp / -0.00390625);
        cv --;
        cv = 0xFFFF ^ cv;
        (*Data)[addr] = static_cast<char>(cv >> 8);
        (*Data)[addr + 1] = static_cast<char>(cv & 255);

    } else {
        cv = static_cast<quint16>(temp / 0.00390625);
        (*Data)[addr] = static_cast<char>(cv >> 8);
        (*Data)[addr + 1] = static_cast<char>(cv & 255);
    }
}

QSNPower QSNRAWtoPower(QByteArray *Data, int addr)
{
    QSNPower ret;
    if (Data->count() <= addr + 5) return ret;
    ret.power = QSNRAWtoInt24(Data, addr) ;
    ret.seconds = QSNRAWtoUInt16(Data, addr + 3);
    return ret;
}

void QSNPowerToRAW(QByteArray *Data, int addr, QSNPower power)
{
    if (Data->count() <= addr + 5) Data->resize(addr + 6);
    QSNInt24ToRAW(Data, addr, power.power);
    QSNUInt16ToRAW(Data, addr + 3, power.seconds);
}

QDateTime QSNRAWtoDateTime(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 6) return QDateTime();
    QDateTime dt;
    dt.setDate(QDate(QSNRAWtoUInt16(Data, addr + 2), static_cast<quint8>(Data->at(addr + 1)) , static_cast<quint8>(Data->at(addr))));
    dt.setTime(QTime(static_cast<quint8>(Data->at(addr + 4)), static_cast<quint8>(Data->at(addr + 5)), static_cast<quint8>(Data->at(addr + 6))));
    // dt.setTimeSpec(Qt::UTC);
    return dt;
}

void QSNDateTimeToRAW(QByteArray *Data, int addr, QDateTime dateTime)
{
    if (Data->count() <= addr + 6) Data->resize(addr + 7);
    (*Data)[addr] = static_cast<char>(dateTime.date().day());
    (*Data)[addr + 1] = static_cast<char>(dateTime.date().month());
    QSNUInt16ToRAW(Data, addr + 2, static_cast<quint16>(dateTime.date().year()));
    (*Data)[addr + 4] = static_cast<char>(dateTime.time().hour());
    (*Data)[addr + 5] = static_cast<char>(dateTime.time().minute());
    (*Data)[addr + 6] = static_cast<char>(dateTime.time().second());
}

QSNNotification QSNRAWtoNotification(QByteArray *Data, int addr)
{
    QSNNotification notif;
    notif.notificationType = 255;
    notif.deviceAddress = 0;
    if (Data->count() <= addr + 2) return notif;
    notif.notificationType = static_cast<quint8>(Data->at(addr));
    notif.deviceAddress = QSNRAWtoUInt16(Data, addr + 1);
    return notif;
}

void QSNNotificationToRAW(QByteArray *Data, int addr, QSNNotification notification)
{
    if (Data->count() <= addr + 2) Data->resize(addr + 3);
    (*Data)[addr] = static_cast<char>(notification.notificationType);
    QSNUInt16ToRAW(Data, addr + 1, notification.deviceAddress);
}

QSNError QSNRAWtoError(QByteArray *Data, int addr)
{
    QSNError err;
    err.errorType = 0;
    err.deviceAddress = 0;
    err.deviceType = 0;
    if (Data->count() <= addr + 7) return err;
    err.errorType = QSNRAWtoUInt32(Data, addr);
    err.deviceAddress = QSNRAWtoUInt16(Data, addr + 4);
    err.deviceType = QSNRAWtoUInt16(Data, addr + 6);
    return err;
}

void QSNErrorToRAW(QByteArray *Data, int addr, QSNError error)
{
    if (Data->count() <= addr + 7) Data->resize(addr + 8);
    QSNUInt32ToRAW(Data, addr, error.deviceAddress);
    QSNUInt16ToRAW(Data, addr + 3, error.deviceAddress);
    QSNUInt16ToRAW(Data, addr + 6, error.deviceType);
}

QString QSNRAWtoASCII(QByteArray *Data, int addr, int textLength) //TODO restruct
{
    QByteArray ascii;
    if (Data->count() < addr + textLength) return ascii;
    for (int i = 0; i < textLength; i ++){
        if (static_cast<quint8>(Data->at(addr + i)) == 0) return ascii;
        ascii.append(static_cast<char>(Data->at(addr + i)));
    }
    return QString::fromLocal8Bit(ascii);
}

void QSNASCIIToRAW(QByteArray *Data, int addr, QString text)
{
    QByteArray ascii = text.toLocal8Bit();
    //  if (text.count() > 7) text.resize(7);
    if (Data->count() <= addr + text.count() - 1) Data->resize(addr + text.count());
    for (int i = 0; i < ascii.count(); i ++)
        (*Data)[addr + i] = ascii[i];
}

QString QSNRAWtoString(QByteArray *Data, QString text, int addr)
{
    QString ret = text;
    if (Data->count() <= addr) return ret;
    int pos = Data->at(addr);
    if (pos > 30) return QSNRAWtoUTF8(Data, addr);
    pos *= 5;
    QString nt = QSNRAWtoUTF8(Data, addr + 1);
    ret.resize(pos + nt.size(), ' ');
    ret.replace(pos, nt.size(), nt);
    return ret;
}


QString QSNRAWtoUTF8(QByteArray *Data, int addr)
{
    QByteArray utf8 = *Data;
    if (Data->count() <= addr) return utf8;
    utf8.remove(0, addr);
    return QString::fromUtf8(utf8);
}

void QSNUTF8ToRAW(QByteArray *Data, int addr, QString text)
{
    QByteArray utf8 = text.toUtf8();
    if (Data->count() <= addr + utf8.count() - 1) Data->resize(addr + utf8.count());
    for (int i = 0; i < utf8.count(); i ++)
        if (addr + i < 8)(*Data)[addr + i] = utf8[i];
}

QDate QSNRAWtoDate(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 3) return QDate();
    QDate date;
    int year;
    year = (static_cast<quint8>(Data->at(addr + 2) << 8)) + static_cast<quint8>(Data->at(addr + 3));
    date.setDate(year, static_cast<quint8>(Data->at(addr + 1)), static_cast<quint8>(Data->at(addr)));
    return date;
}

void QSNDateToRAW(QByteArray *Data, int addr, QDate date)
{
    if (Data->count() <= addr + 3) Data->resize(addr + 4);
    quint16 year = static_cast<quint16>(date.year());
    (*Data)[addr] = static_cast<char>(date.day());
    (*Data)[addr + 1] = static_cast<char>(date.month());
    QSNUInt16ToRAW(Data, addr + 2, year);
}

QTime QSNRAWtoTime(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 2) return QTime();
    QTime time;
    time.setHMS(static_cast<quint8>(Data->at(addr)), static_cast<quint8>(Data->at(addr + 1)), static_cast<quint8>(Data->at(addr + 2)));
    return time;
}

void QSNTimeToRAW(QByteArray *Data, int addr, QTime time)
{
    if (Data->count() <= addr + 2) Data->resize(addr + 3);
    (*Data)[addr] = static_cast<char>(time.hour());
    (*Data)[addr + 1] = static_cast<char>(time.minute());
    (*Data)[addr + 2] = static_cast<char>(time.second());
}

QString QSNRAWtoHEXcode(QByteArray *Data, int addr)
{
    if (Data->count() <= addr + 3) return QLatin1String("00000000h");
    QString ret = QString("%1").arg(QString().number(QSNRAWtoUInt32(Data, 1), 16)).toUpper();
    ret = ret.prepend(QString().fill('0', 8 - ret.length())) + 'h';
    return ret;
}

void QSNHEXcodeToRAW(QByteArray *Data, int addr, QString HEXstring)
{
    if (Data->count() <= addr + 3) Data->resize(addr + 4);
    bool res = false;
    QSNUInt32ToRAW(Data, addr, HEXstring.toULong(&res, 16));
}

QSNSNIR QSNRAWtoSNIR(QByteArray *Data, int addr)
{
    QSNSNIR ret;
    if (Data->count() <= addr + 6) {
        ret.address = 0;
        ret.parameter = 0;
        ret.Data = QByteArray(6, 0);
        return ret;
    }
    ret.address = static_cast<quint8>((*Data)[addr] >> 3);
    ret.parameter = ((*Data)[addr] & 7);
    for (int i = 0; i < 6; i ++)
        ret.Data.append(Data->at(i + addr + 1));
    return ret;
}

void QSNSNIRToRAW(QByteArray *Data, int addr, QSNSNIR snir) {
    if (Data->count() <= addr + 6) Data->resize(addr + 7);
    (*Data)[addr] = static_cast<char>((snir.address << 3) + (snir.parameter & 7));
    for (int i = 0; i < 6; i ++)
        (*Data)[addr + 1 + i] = snir.Data[i];
}

QSNEMUID QSNRAWtoEmUID(QByteArray *Data, int addr)
{
    QSNEMUID ret;
    if (Data->count() <= addr + 3) {
        ret.family = 0;
        ret.number = 0;
        return ret;
    }
    ret.family = QSNRAWtoUInt16(Data, addr);
    ret.number = QSNRAWtoUInt16(Data, addr + 2);
    return ret;
}

void QSNEmUIDToRAW(QByteArray *Data, int addr, QSNEMUID uid)
{
    if (Data->count() <= addr + 3) Data->resize(addr + 4);
    QSNUInt16ToRAW(Data, addr, uid.family);
    QSNUInt16ToRAW(Data, addr + 2, uid.number);
}

QString QSNRAWtoVersion(QByteArray *Data, int addr)
{
    QString ver;
    int c = Data->count() - addr;
    for (int i = 0; i < c; i ++)
        ver += QString::number(Data->at(addr + i)) + '.';
    if (ver.right(1) == QString(".")) ver.remove(ver.count() - 1, 1);
    return ver;
}

void QSNVersionToRAW(QByteArray *Data, int addr, QString version)
{
    bool ok;
    int a = addr;
    QStringList sl = version.split('.');
    if (Data->count() < addr + sl.count()) Data->resize(addr + sl.count());
    foreach(QString item, sl) if (a < 8)(*Data)[a ++] = static_cast<char>(item.toInt(&ok));
}

//=========================containers==============================================

QSNContainer newContainer()
{
    QSNContainer cn;
    cn.Address = 0;
    cn.Command = 0;
    cn.Data = QByteArray();
    cn.Sender = 0;
    cn.Signal = 0;
    cn.info = QString();
    return cn;
}

void containerToStream(QSNContainer *container, QDataStream *stream)
{
    *stream << static_cast<int>(container->role);
    *stream << container->Signal;
    *stream << container->Command;
    *stream << container->Data;
    *stream << container->Address;
    *stream << container->Sender;
    *stream << container->info;
}

QSNContainer containerFromStream(QDataStream *stream)
{
    QSNContainer container = newContainer();
    int role;
    *stream >> role;
    container.role = static_cast<QSNContainer::type>(role);
    *stream >> container.Signal;
    *stream >> container.Command;
    *stream >> container.Data;
    *stream >> container.Address;
    *stream >> container.Sender;
    *stream >> container.info;
    return container;
}

void dataEncript(QByteArray *data, int start, QString key)
{
    QByteArray keyarray = key.toLocal8Bit();
    int k = 0;
    int kl = keyarray.count() - 1;
    data->append(static_cast<char>(0));
    int c = data->count() - 1;
    char s = 0;
    char se = 0;
    for (int i = start; i <= c; i ++) {
        se = s;
        if (i < c) s = s ^ (*data)[i];
        else (*data)[i] = s;
        (*data)[i] = (*data)[i] ^ keyarray[k];
        (*data)[i] = (*data)[i] ^ se;
        (*data)[i] = (*data)[i] ^ keyarray[kl - k];
        k ++;
        if (k > kl) k = 0;
    }
}

bool dataDecript(QByteArray *data, int start, QString key)
{
    QByteArray keyarray = key.toLocal8Bit();
    int k = 0;
    int kl = keyarray.count() - 1;
    int c = data->count() - 1;
    char s = 0;
    for (int i = start; i <= c; i ++) {
        (*data)[i] = (*data)[i] ^ keyarray[k];
        (*data)[i] = (*data)[i] ^ s;
        (*data)[i] = (*data)[i] ^ keyarray[kl - k];
        if (i < c) s = s ^ (*data)[i];
        k ++;
        if (k > kl) k = 0;
    }
    if (s == (*data)[c]) return true;
    return false;
}

QSNDefaultChannel QSNDefChanFromType(quint8 type)
{
    QSNDefaultChannel ret;
    switch (type) {
    case 0:
        ret.defaultName = QObject::tr("No type");
        ret.defaultSignal = 65000;
        ret.defaultNote = QObject::tr("Description channel");
        ret.defaultSignalType = 0;
        break;
    case 1:
        ret.defaultName = QObject::tr("Departure");
        ret.defaultSignal = 50001;
        ret.defaultNote = QObject::tr("The signal informs all devices on the long-term absence");
        ret.defaultSignalType = 0;
        break;
    case 2:
        ret.defaultName = QObject::tr("Presence");
        ret.defaultSignal = 50002;
        ret.defaultNote = QObject::tr("The signal notifies the device presence.");
        ret.defaultSignalType = 0;
        break;
    case 3:
        ret.defaultName = QObject::tr("Absence");
        ret.defaultSignal = 50003;
        ret.defaultNote = QObject::tr("The signal notifies the device of the absence.");
        ret.defaultSignalType = 0;
        break;
    case 4:
        ret.defaultName = QObject::tr("Nighttime");
        ret.defaultSignal = 50004;
        ret.defaultNote = QObject::tr("The signal notifies the device of night time.");
        ret.defaultSignalType = 0;
        break;
    case 5:
        ret.defaultName = QObject::tr("No nighttime");
        ret.defaultSignal = 50005;
        ret.defaultNote = QObject::tr("The signal notifies the device is not on a night time.");
        ret.defaultSignalType = 0;
        break;
    case 6:
        ret.defaultName = QObject::tr("Daytime");
        ret.defaultSignal = 50006;
        ret.defaultNote = QObject::tr("The signal notifies the device of day time.");
        ret.defaultSignalType = 0;
        break;
    case 7:
        ret.defaultName = QObject::tr("Not daytime");
        ret.defaultSignal = 50007;
        ret.defaultNote = QObject::tr("The signal notifies the device is not on a day time.");
        ret.defaultSignalType = 0;
        break;
    case 8:
        ret.defaultName = QObject::tr("Errors");
        ret.defaultSignal = 50008;
        ret.defaultNote = QObject::tr("Channel errors, transmits the address and the error code.");
        ret.defaultSignalType = 15;
        break;
    case 9:
        ret.defaultName = QObject::tr("Alert");
        ret.defaultSignal = 50009;
        ret.defaultNote = QObject::tr("A signal for notifying occurrence of an alarm event.");
        ret.defaultSignalType = 14;
        break;
    case 10:
        ret.defaultName = QObject::tr("Time");
        ret.defaultSignal = 50010;
        ret.defaultNote = QObject::tr("The time signal, the device transmits to the clock or the device receives the time signal.");
        ret.defaultSignalType = 12;
        break;
    case 11:
        ret.defaultName = QObject::tr("Security alarm");
        ret.defaultSignal = 50011;
        ret.defaultNote = QObject::tr("A signal for security occurrence of an security alarm event.");
        ret.defaultSignalType = 31;
        break;
    case 12:
        ret.defaultName = QObject::tr("Bell");
        ret.defaultSignal = 50012;
        ret.defaultNote = QObject::tr("A signal notifies the device of bell event.");
        ret.defaultSignalType = 0;
        break;
    case 13:
        ret.defaultName = QObject::tr("Silent mode");
        ret.defaultSignal = 50013;
        ret.defaultNote = QObject::tr("The signal sets the device to silent mode.");
        ret.defaultSignalType = 1;
        break;
    case 14:
        ret.defaultName = QObject::tr("Temperature");
        ret.defaultSignal = 50014;
        ret.defaultNote = QObject::tr("Signal transmits temperature, with the sender's IDP.");
        ret.defaultSignalType = 9;
        break;
    case 15:
        ret.defaultName = QObject::tr("Electricity");
        ret.defaultSignal = 50015;
        ret.defaultNote = QObject::tr("The signal transmits the consumed electricity, with the sender's IDP.");
        ret.defaultSignalType = 19;
        break;
    }
    return ret;
}

QStringList QSNDefChanListLabels()
{
    QStringList list;
    QString chName;
    quint8 i = 0;
    do {
        chName = QSNDefChanFromType(i).defaultName;
        if (!chName.isEmpty()) list.append(chName);
        i ++;
    } while (!chName.isEmpty() );
    return list;
}


QDir QSNHomeDir()
{
    QDir HomeDir = QDir::home();
    const QString Folder = QLatin1String("signalnet");
    if (!HomeDir.exists(Folder)) HomeDir.mkpath(Folder);
    HomeDir.cd(Folder);
    return HomeDir;
}

QDir QSNHomePath(QString path)
{
    QDir HomeDir = QSNHomeDir();
    if (!HomeDir.exists(path)) HomeDir.mkpath(path);
    HomeDir.cd(path);
    return HomeDir;
}

QDir QSNHomeSubPath(QString path, QString sub)
{
    QDir HomeDir = QSNHomePath(path);
    if (!HomeDir.exists(sub)) HomeDir.mkpath(sub);
    HomeDir.cd(sub);
    return HomeDir;
}

QString QSNNormalizationFileName(QString name)
{
    QString ret = name.toLower();
    for (int i = 0; i < ret.count(); i ++)
    {
        if (ret[i] == ' ') ret[i] = '_';
        if (ret[i] == ',') ret[i] = '_';
        if (ret[i] == '.') ret[i] = '_';
        if (ret[i] == ';') ret[i] = '_';
        if (ret[i] == ':') ret[i] = '_';
    }
    return ret;
}

int QSNmemorySizeToBlockCount(int memorySize)
{
    int blocks = memorySize / 6;
    if (blocks * 6 < memorySize) blocks ++;
    return blocks;
}

QString QSNAngleToString(qint16 minutes)
{
    qint16 deg = minutes / 60;
    quint8 min = static_cast<quint8>(abs(minutes - deg * 60));
    if (min == 0) return QString("%1°").arg(deg);
    else return QString("%1°%2").arg(deg).arg(min);
}

bool QSNCompareVersion(QString ver1, QString ver2)
{
    return (ver1.left(ver1.lastIndexOf('.')) == ver2.left(ver2.lastIndexOf('.')));
}

QDateTime QSNStrToDateTime(QString strDT)
{
    return QDateTime::fromString(strDT, QLatin1String("yyyy-M-d_h-m"));
}

QString QSNStrToDateTime(QDateTime DT)
{
    return DT.toString(QLatin1String("yyyy-MM-d_h-mm"));
}

QString QSNDayWeekAbbreviated(int day)
{
    switch (day) {
    case 1: return QObject::tr("Mo");
    case 2: return QObject::tr("To");
    case 3: return QObject::tr("We");
    case 4: return QObject::tr("Th");
    case 5: return QObject::tr("Fr");
    case 6: return QObject::tr("Sa");
    case 7: return QObject::tr("Su");
    default: return QObject::tr("none");
    }
}

QString QSNDaysWeekToLine(quint8 week)
{
    QString r;
    for (int i = 0; i < 7; i ++)
        if (week & (static_cast<quint8>(1) << i)) {
            if (!r.isEmpty()) r += ",";
            r += QSNDayWeekAbbreviated(i + 1);
        }
    return r;
}

QString QSNNameNormalization(QString name)
{
    QString ret = name;
    for (int i = 0; i < ret.count(); i ++)  {
        if (ret[i] == ',') ret[i] = '_';
        if (ret[i] == '.') ret[i] = '_';
        if (ret[i] == ';') ret[i] = '_';
        if (ret[i] == ':') ret[i] = '_';
        if (ret[i] == ' ') {
            ret.remove(i, 1);
            if (i < ret.count())
                ret[i] = ret[i].toUpper();
        }
    }
    return ret;
}

QString QSNBoolToText(bool state)
{
    if (state) return QLatin1String("true");
    return QLatin1String("false");
}

bool QSNTextToBool(QString state)
{
    if (state.contains("true", Qt::CaseInsensitive)) return true;
    return false;
}

QString QSNBoolToYesNo(bool state)
{
    if (state) return QObject::tr("yes");
    return QObject::tr("no");
}

QString QSNDecodeText(QString text)
{
    QByteArray ret;
    QString source = text;
    bool bStatus = false;
    int i = 0;
    while (i < source.length()) {
        if (source[i] != '%') {
            ret.append(source[i].toLatin1());
            i ++;
        } else {
            ret.append(static_cast<char>(source.mid(i + 1, 2).toInt(&bStatus, 16)));
            i += 3;
        }
    }
    return QString::fromLocal8Bit(ret);
}

QString QSNCleanIP(QString address)
{
    QString a = address;
    if (a.left(7) == QString("::ffff:​").left(7)) a = a.remove(0, 7);
    else if (a.left(3) == QString("::1​").left(3)) {
        a = a.remove(0, 3);
        a = a.prepend("loopback");
    } else if (a.left(6) == QString("fc00::​").left(6)) {
        a = a.remove(0, 6);
        a = a.prepend("Unique Local Unicast");
    } if (a.left(6) == QString("ff00::​").left(6)) {
        a = a.remove(0, 6);
        a = a.prepend("multicast");
    }
    return a;
}

QString QSNAllocateNumberFromString(QString str)
{
    int start=str.indexOf(QRegExp("[\\+\\-]?[0-9]"));
    int finish=str.lastIndexOf(QRegExp("[\\+\\-]?[0-9]"));
    return str.mid(start, finish-start + 1);
}

QString QSNUpTime(QDateTime begin)
{
    long long ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - begin.toMSecsSinceEpoch();
    quint32 days = static_cast<quint32>(ms / 86400000);
    ms = ms - (days * 86400000);
    if (days == 0) return QTime::fromMSecsSinceStartOfDay(static_cast<int>(ms)).toString("hh:mm");
    return QString("%1 %2 - %3").arg(days).arg(QObject::tr("days")).arg(QTime::fromMSecsSinceStartOfDay(static_cast<int>(ms)).toString("hh:mm"));
}

QString QSNNameToFileName(QString str)
{
    QString fn;
    int i, rU, rL;
    QString validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_,.()[]{}<>~!@#$%^&*+=?";
    QString rusUpper = QObject::tr("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
    QString rusLower = QObject::tr("абвгдеёжзийклмнопрстуфхцчшщыэюя");
    QStringList latUpper, latLower;
    latUpper <<"A"<<"B"<<"V"<<"G"<<"D"<<"E"<<"Jo"<<"Zh"<<"Z"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"
            <<"O"<<"P"<<"R"<<"S"<<"T"<<"U"<<"F"<<"H"<<"C"<<"Ch"<<"Sh"<<"Sh"<<"I"<<"E"<<"Ju"<<"Ja";
    latLower <<"a"<<"b"<<"v"<<"g"<<"d"<<"e"<<"jo"<<"zh"<<"z"<<"i"<<"j"<<"k"<<"l"<<"m"<<"n"
            <<"o"<<"p"<<"r"<<"s"<<"t"<<"u"<<"f"<<"h"<<"c"<<"ch"<<"sh"<<"sh"<<"i"<<"e"<<"ju"<<"ja";
    for (i=0; i < str.size(); ++i){
        if ( validChars.contains(str[i]) ){
            fn = fn + str[i];
        } else if (str[i] == ' '){  //replace spaces
            fn = fn + "_";
        } else{
            rU = rusUpper.indexOf(str[i]);
            rL = rusLower.indexOf(str[i]);
            if (rU > 0) fn = fn + latUpper[rU];
            else if (rL > 0) fn = fn + latLower[rL];
            else fn = fn + str[i];
        }
    }
    if (fn.isEmpty() ) fn = "file";
    return fn;
}

QString QSNWarningLevelToText(quint8 level)
{
    switch (level) {
    case 0: return QString("(!)");
    case 1: return QString("/!\\");
    case 2: return QString("(i)");
    case 3: return QString("(*)");
    }
    return QString("(?)");
}

QString QSNWarningLevelToLabel(quint8 level)
{
    switch (level) {
    case 0: return QObject::tr("Caution");
    case 1: return QObject::tr("Warning");
    case 2: return QObject::tr("Information");
    case 3: return QObject::tr("Note");
    }
    return QObject::tr("Tip");
}

quint8 QSNWarningLevelFromText(QString text)
{
    if (text.indexOf("(!)") != -1) return 0;
    if (text.indexOf("/!\\") != -1) return 1;
    if (text.indexOf("(i)") != -1) return 2;
    if (text.indexOf("(*)") != -1) return 3;
    return 4;
}

QString QSNGetWarninMSGNote(QString msg)
{
    QString note = msg;
    int i = note.indexOf("{");
    if (i == -1) note = QString();
    else {
        note.remove(0, i + 1);
        i = note.indexOf("}");
        note.remove(i, note.count() - i);
        if (i == -1) note = QString();
    }
    return note;
}

QString QSNGetWarninMSGBody(QString msg)
{
    QString text = msg;
    int i = text.indexOf("\\");
    if (i >= 2) text.remove(i - 2, 3);
    i = text.indexOf(")");
    if (i >= 2) text.remove(i - 2, 3);
    i = text.indexOf("{");
    if (i == -1) return text;
    text.remove(i, text.count() - i);
    return text;
}

QString QSNEscapingHTMLCharacters(QString text)
{
    QString ret;
    QChar c;
    for (int i = 0; i < text.count(); i ++) {
        c = text[i];
        if (c == '"') ret.append("&Prime;");
        else if (c == '\'') ret.append("&prime;");
        else ret.append(c);
    }
    return ret;
}

QString QSNGetSerialErrorName(int error)
{
    switch(error)
    {
    case 1: return QObject::tr("Device not found error");
    case 2: return QObject::tr("Permission error");
    case 3: return QObject::tr("Open error");
    case 4: return QObject::tr("Parity error");
    case 5: return QObject::tr("Framing error");
    case 6: return QObject::tr("Break condition error");
    case 7: return QObject::tr("Write error");
    case 8: return QObject::tr("Read error");
    case 9: return QObject::tr("Resource error");
    case 10: return QObject::tr("Unsupported operation error");
    case 11: return QObject::tr("Unknown error");
    case 12: return QObject::tr("Timeout error");
    case 13: return QObject::tr("Not open error");

    default:
        return QObject::tr("No error occurred");
    }
}

QString QSNContainerToLogText(QSNContainer container, bool directionIn)
{
    if (container.role == QSNContainer::information) return QObject::tr("Information");
    QString text;
    if (directionIn) text += QObject::tr("recive");
    else text += QObject::tr("send");
    if (container.role == QSNContainer::signal) {
        text += " " + QObject::tr("signal") + " " + QString::number(container.Signal) +
                " " + QObject::tr("data") + " " + container.Data.toHex();
    }
    if (container.role == QSNContainer::message) {
        text += " " + QObject::tr("message") + " " + QSNMessageTypeToName(container.Command) +
                " " + QObject::tr("data") + " " + container.Data.toHex();
    }
    if (container.role == QSNContainer::service) {
        text += " " + QObject::tr("service");
    }
    return text;
}

QString QSNMessageTypeToName(quint8 type)
{
    switch (type)
    {
    case 0: return QObject::tr("get info");
    case 1: return QObject::tr("new device");
    case 2: return QObject::tr("get memory");
    case 3: return QObject::tr("read memory");
    case 4: return QObject::tr("set memory");
    case 5: return QObject::tr("write memory");
    case 6: return QObject::tr("set address");
    case 7: return QObject::tr("answer address");
    case 9: return QObject::tr("answer parametr");

    }
    return QObject::tr("not known");
}
