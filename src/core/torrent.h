/* - DownZemAll! - Copyright (C) 2019-present Sebastien Vavassori
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CORE_TORRENT_H
#define CORE_TORRENT_H

#include <Core/TorrentMessage>

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSet>
#include <QtCore/QSortFilterProxyModel>

class TorrentFileTableModel;
class TorrentPeerTableModel;
class TorrentTrackerTableModel;


class Torrent : public QObject
{
    Q_OBJECT

public:
    explicit Torrent(QObject *parent = Q_NULLPTR);
    ~Torrent() Q_DECL_OVERRIDE = default;

    void clear();
    bool isEmpty();

    QString url() const;
    void setUrl(const QString &url);

    QString localFullFileName() const;
    void setLocalFullFileName(const QString &filename);

    QString localFilePath() const;
    void setLocalFilePath(const QString &outputPath);

    /* Metadata and info */
    QString status() const;

    TorrentMetaInfo metaInfo() const;
    void setMetaInfo(const TorrentMetaInfo &metaInfo);

    TorrentInfo info() const;
    void setInfo(const TorrentInfo &info, bool mustRefreshMetaInfo);

    TorrentHandleInfo detail() const;
    void setDetail(const TorrentHandleInfo &detail, bool mustRefreshMetaInfo);

    int progress() const;

    void setError(TorrentError::Type errorType, const QString &message);

    int fileCount() const;

    TorrentFileInfo::Priority filePriority(int index) const;
    void setFilePriority(int index, TorrentFileInfo::Priority priority);

    QString preferredFilePriorities() const;
    void setPreferredFilePriorities(const QString &priorities);

    void addPeer(const QString &input);
    void removeUnconnectedPeers();

    int trackerCount() const;
    void addTracker(const QString &url);
    void removeTrackerAt(int index);


    /* Table Models */
    QAbstractTableModel* fileModel() const;
    QAbstractTableModel* peerModel() const;
    QAbstractTableModel* trackerModel() const;

    void retranslateUi();

signals:
    void changed();

private:
    QString m_url;
    QString m_localTorrentFileName;
    QString m_outputPath;

    TorrentMetaInfo m_metaInfo;
    TorrentInfo m_info;
    TorrentHandleInfo m_detail;

    TorrentFileTableModel* m_fileModel;
    TorrentPeerTableModel* m_peerModel;
    TorrentTrackerTableModel* m_trackerModel;
};

/******************************************************************************
 ******************************************************************************/
class SortFilterProxyModel: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModel(QObject *parent = Q_NULLPTR);
};

/******************************************************************************
 ******************************************************************************/
class AbstractTorrentTableModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Role {
        ProgressRole = Qt::UserRole + 1, ///< The progress value. (int, between 0 and 100)
        SegmentRole, ///< The data to render the segments. (QBitArray)
        ConnectRole, ///< The connection state of the peer or tracker. (bool)
        SortRole
    };

    explicit AbstractTorrentTableModel(Torrent *parent = Q_NULLPTR);

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

protected:
    QStringList m_headers;
};

/******************************************************************************
 ******************************************************************************/
class TorrentFileTableModel: public AbstractTorrentTableModel
{
    Q_OBJECT
public:
    explicit TorrentFileTableModel(Torrent *parent = Q_NULLPTR);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void refreshMetaData(const QList<TorrentFileMetaInfo> &files);
    void refreshData(const QList<TorrentFileInfo> &files);
    void retranslateUi();

private:
    QList<TorrentFileMetaInfo> m_filesMeta;
    QList<TorrentFileInfo> m_files;

    qsizetype m_pieceByteSize = 0;
    QBitArray m_downloadedPieces;
    int percent(const TorrentFileMetaInfo &mi, const TorrentFileInfo &ti) const;
    qint64 firstPieceIndex(const TorrentFileMetaInfo &mi) const;
    qint64 lastPieceIndex(const TorrentFileMetaInfo &mi) const;
    qint64 startBlockInFirstPiece(const TorrentFileMetaInfo &mi) const;
    qint64 pieceCount(const TorrentFileMetaInfo &mi) const;
    QBitArray pieceSegments(const TorrentFileMetaInfo &mi) const;
};

/******************************************************************************
 ******************************************************************************/
/*!
 * \class TorrentPeerTableModel
 * \brief List of peers and various information about them
 */
class TorrentPeerTableModel: public AbstractTorrentTableModel
{
    Q_OBJECT
public:
    explicit TorrentPeerTableModel(Torrent *parent = Q_NULLPTR);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void refreshData(const QList<TorrentPeerInfo> &peers);
    void retranslateUi();

    void removeUnconnectedPeers();

private:
    QList<TorrentPeerInfo> m_peers;
    QSet<EndPoint> m_connectedPeers;

    void appendRemainingSafely(const QList<TorrentPeerInfo> &peers);
};

/******************************************************************************
 ******************************************************************************/
class TorrentTrackerTableModel: public AbstractTorrentTableModel
{
    Q_OBJECT
public:
    explicit TorrentTrackerTableModel(Torrent *parent = Q_NULLPTR);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void refreshData(const QList<TorrentTrackerInfo> &trackers);
    void retranslateUi();

private:
    QList<TorrentTrackerInfo> m_trackers;
};


#endif // CORE_TORRENT_H
