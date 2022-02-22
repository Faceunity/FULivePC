#include "ImageProvider.h"

ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    return this->m_showImage;
}

