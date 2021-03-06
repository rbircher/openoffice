/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

#ifndef _SVX_GALLERYITEMITEM_HXX
#define _SVX_GALLERYITEMITEM_HXX

#include <svx/svxdllapi.h>
#include <svl/poolitem.hxx>

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/lang/XComponent.hpp>

// property names map those from css::gallery::GalleryItem
// with exception of "AsLink" and "FilterName"
#define SVXGALLERYITEM_TYPE     "GalleryItemType"
#define SVXGALLERYITEM_LINK     "AsLink"
#define SVXGALLERYITEM_URL      "URL"
#define SVXGALLERYITEM_FILTER   "FilterName"
#define SVXGALLERYITEM_DRAWING  "Drawing"
#define SVXGALLERYITEM_GRAPHIC  "Graphic"
#define SVXGALLERYITEM_PARAMS   6
#define SVXGALLERYITEM_ARGNAME  "GalleryItem"

DBG_NAMEEX_VISIBILITY( SvxGalleryItem, SVX_DLLPUBLIC )

class SVX_DLLPUBLIC SvxGalleryItem : public SfxPoolItem
{
    sal_Int8 m_nType;
    sal_Bool m_bIsLink;
    rtl::OUString m_aURL;
    rtl::OUString m_aFilterName;
    com::sun::star::uno::Reference< com::sun::star::lang::XComponent > m_xDrawing;
    com::sun::star::uno::Reference< com::sun::star::graphic::XGraphic > m_xGraphic;

public:
    TYPEINFO();

    SvxGalleryItem();
    SvxGalleryItem( const SvxGalleryItem& );
    SvxGalleryItem( const sal_uInt16 nId );
    ~SvxGalleryItem();

    sal_Int8 GetType() const { return m_nType; }
    sal_Bool IsLink() const { return m_bIsLink; }
    const rtl::OUString GetURL() const { return m_aURL; }
    const rtl::OUString GetFilterName() const { return m_aFilterName; }
    const com::sun::star::uno::Reference< com::sun::star::lang::XComponent > GetDrawing() const { return m_xDrawing; }
    const com::sun::star::uno::Reference< com::sun::star::graphic::XGraphic > GetGraphic() const { return m_xGraphic; }

    // pure virtual methods from SfxPoolItem
    virtual int          operator==( const SfxPoolItem& ) const;
    virtual SfxPoolItem* Clone( SfxItemPool *pPool = 0 ) const;
    // bridge to UNO
    virtual sal_Bool     QueryValue( com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 ) const;
    virtual sal_Bool     PutValue( const com::sun::star::uno::Any& rVal, sal_uInt8 nMemberId = 0 );
    // not implemented
    virtual SfxPoolItem* Create(SvStream &, sal_uInt16) const;
    virtual SvStream&    Store(SvStream &, sal_uInt16 nItemVersion) const;
};

#endif
