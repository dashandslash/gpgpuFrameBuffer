//
//  gpGpuFrameBuffer.h
//  dsFluid
//
//  Created by luca lolli on 16/12/2015.
//
//

#ifndef gpGpuFrameBuffer_h
#define gpGpuFrameBuffer_h


#include "cinder/cinder.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

typedef shared_ptr<class gpGpuFrameBuffer> gpGpuFrameBufferRef;

class gpGpuFrameBuffer{
    
public:
    static gpGpuFrameBufferRef create(int width, int height, GLint colorFormat = GL_RGBA32F)
    {
        return gpGpuFrameBufferRef( new gpGpuFrameBuffer{width, height, colorFormat});
    }
    
    static gpGpuFrameBufferRef create(ivec2 size, GLint colorFormat = GL_RGBA32F)
    {
        return gpGpuFrameBufferRef( new gpGpuFrameBuffer{size.x, size.y, colorFormat});
    }

    gpGpuFrameBuffer(int width, int height, GLint colorFormat = GL_RGBA32F):mWidth(width), mHeight(height), mColorFormat(colorFormat)
    {
        initFbo();
    }
    
    gpGpuFrameBuffer(ivec2 size, GLint colorFormat = GL_RGBA32F )
    {
        
        gpGpuFrameBuffer(size.x, size.y, colorFormat);
        
    }
    
    ~gpGpuFrameBuffer(){}
    
    void initFbo()
    {
        mTextureFormat.setMagFilter( GL_NEAREST );
        mTextureFormat.setMinFilter( GL_NEAREST );
        mTextureFormat.setWrap(GL_REPEAT, GL_REPEAT);
        mTextureFormat.setInternalFormat( mColorFormat );
        
        //Create fbo with two attachments for faster ping pong
        mFboFormat.attachment( GL_COLOR_ATTACHMENT0, gl::Texture2d::create( mWidth, mHeight, mTextureFormat ) )
        .attachment( GL_COLOR_ATTACHMENT1, gl::Texture2d::create( mWidth, mHeight, mTextureFormat ) );
        
        mReadIndex	= GL_COLOR_ATTACHMENT0;
        mWriteIndex = GL_COLOR_ATTACHMENT1;
        mFbo		= gl::Fbo::create(mWidth, mHeight, mFboFormat );
        
        clear();
    }
    
    void clear()
    {
        mFbo->bindFramebuffer();
        
        const GLenum buffers[ 2 ] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        
        gl::drawBuffers( 2, buffers );
        
        gl::viewport( mFbo->getSize() );
        
        gl::clear(Color(0.0,0.0,0.0));
        
        mFbo->unbindFramebuffer();
    }
    
    gl::FboRef getBuffer()
    {
        return mFbo;
    }
    
    GLint getBufferLocation()
    {
        return mWriteIndex;
    }
    
    GLint getTextureLocation()
    {
        return mWriteIndex;
    }
    
    void drawBuffer()
    {
        gl::drawBuffer( mWriteIndex );
    }
    
    void bindTexture(int textureUnit = 0)
    {
        mFbo->getTexture2d(mReadIndex)->bind(textureUnit);
    }
    
    void unbindTexture()
    {
        mFbo->getTexture2d(mReadIndex)->unbind();
    }
    
    void bindBuffer()
    {
        //        mWriteIndex = ( mReadIndex + 1 ) % 2;
        mFbo->bindFramebuffer();
        gl::drawBuffer( mWriteIndex );
    }
    
    void unbindBuffer(bool toSwap = false)
    {
        mFbo->unbindFramebuffer();
        
        if(toSwap)
        {
            swap();
        }
    }
    
    
    gl::TextureRef getTexture()
    {
        return mFbo->getTexture2d(mReadIndex);
    }
    
    void swap(){
        std::swap(mReadIndex,mWriteIndex);
    }
    
    void draw()
    {
        gl::draw( mFbo->getTexture2d( mReadIndex ) );
    }
    
    ivec2 getSize()
    {
        return ivec2(mWidth, mHeight);
    }
    
private:
    GLint                   mColorFormat;
    int                     mWidth, mHeight;
    
    gl::Fbo::Format         mFboFormat;
    gl::Texture::Format     mTextureFormat;
    
    GLenum                  mReadIndex, mWriteIndex;
    
    gl::FboRef              mFbo;
    
    
};

#endif /* gpGpuFrameBuffer_h */
