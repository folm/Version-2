// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef FOLM_QT_FOLMADDRESSVALIDATOR_H
#define FOLM_QT_FOLMADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class FolmAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit FolmAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Folm address widget validator, checks for a valid folm address.
 */
class FolmAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit FolmAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // FOLM_QT_FOLMADDRESSVALIDATOR_H
