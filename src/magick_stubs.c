/* Copyright (C) 2022  Florent Monnier
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to modify it and redistribute it
 * freely.
 *
 * This software is provided "AS-IS", without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include <magick/api.h>

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/custom.h>
#include <caml/fail.h>


CAMLprim value
caml_InitializeMagick(value unit)
{
    InitializeMagick(NULL);
    return Val_unit;
}

CAMLprim value
caml_DestroyMagick(value unit)
{
    DestroyMagick();
    return Val_unit;
}

#if 1

static value Val_Image(Image *img)
{
    value v = caml_alloc(1, Abstract_tag);
    *((Image **) Data_abstract_val(v)) = img;
    return v;
}

static Image * Image_val(value v)
{
    return *((Image **) Data_abstract_val(v));
}

#else

static value Val_Image(Image *img)
{
    return caml_copy_nativeint((intnat) img);
}

static Image * Image_val(value v)
{
    return (Image *) Nativeint_val(v);
}
#endif


CAMLprim value
caml_ReadImage(value img_filename)
{
    CAMLparam1(img_filename);

    Image *image = (Image *) NULL;
    char infile[MaxTextExtent];
    ImageInfo *imageInfo;
    ExceptionInfo exception;

    imageInfo = CloneImageInfo((ImageInfo *) NULL);
    GetExceptionInfo(&exception);

    (void) strncpy(infile, String_val(img_filename), MaxTextExtent-1);
    (void) strcpy(imageInfo->filename, infile);

    image = ReadImage(imageInfo, &exception);

    if (imageInfo != (ImageInfo *) NULL)
        DestroyImageInfo(imageInfo);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        DestroyExceptionInfo(&exception);
        caml_failwith("Magick.read_image");
    }

    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_WriteImage(value _image, value out_filename)
{
    CAMLparam2(_image, out_filename);

    Image *image = (Image *) NULL;
    char outfile[MaxTextExtent];
    ImageInfo *imageInfo;
    ExceptionInfo exception;

    imageInfo = CloneImageInfo((ImageInfo *) NULL);
    GetExceptionInfo(&exception);

    image = Image_val(_image);

    (void) strncpy(outfile, String_val(out_filename), MaxTextExtent-1);

    (void) strcpy(image->filename, outfile);

    if (!WriteImage(imageInfo, image))
    {
        CatchException(&image->exception);
        if (image != (Image *) NULL) DestroyImage(image);
        if (imageInfo != (ImageInfo *) NULL) DestroyImageInfo(imageInfo);
        caml_failwith("Magick.write_image");
    }

    CAMLreturn(Val_unit);
}

CAMLprim value
caml_BlobToImage(value blob)
{
    CAMLparam1(blob);

    Image *image = (Image *) NULL;
    ImageInfo *imageInfo;
    ExceptionInfo exception;

    imageInfo = CloneImageInfo((ImageInfo *) NULL);
    GetExceptionInfo(&exception);

    image = BlobToImage(imageInfo, String_val(blob), caml_string_length(blob), &exception);

    if (imageInfo != (ImageInfo *) NULL)
        DestroyImageInfo(imageInfo);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        DestroyExceptionInfo(&exception);
        caml_failwith("Magick.blob_to_image");
    }

    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_ImageWidth(value _image)
{
    CAMLparam1(_image);
    Image *image = Image_val(_image);
    CAMLreturn(Val_int(image->columns));
}

CAMLprim value
caml_ImageHeight(value _image)
{
    CAMLparam1(_image);
    Image *image = Image_val(_image);
    CAMLreturn(Val_int(image->rows));
}

CAMLprim value
caml_DisplayImages(value _image)
{
    CAMLparam1(_image);

    Image *image = (Image *) NULL;
    image = Image_val(_image);

    ImageInfo *imageInfo = CloneImageInfo((ImageInfo *) NULL);

    if (!DisplayImages(imageInfo, image))
    {
        CatchException(&image->exception);
        DestroyImageInfo(imageInfo);
        caml_failwith("Magick.display");
    }
    DestroyImageInfo(imageInfo);
    CAMLreturn(Val_unit);
}

CAMLprim value
caml_DestroyImage(value _image)
{
    CAMLparam1(_image);

    DestroyImage(Image_val(_image));

    CAMLreturn(Val_unit);
}

CAMLprim value
caml_CloneImage(value _image)
{
    CAMLparam1(_image);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = CloneImage(Image_val(_image), 0, 0, 1, &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.clone");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_BlurImage(value _image, value radius, value sigma)
{
    CAMLparam3(_image, radius, sigma);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = BlurImage(Image_val(_image), Double_val(radius), Double_val(sigma), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.blur");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_MotionBlurImage(value _image, value radius, value sigma, value angle)
{
    CAMLparam4(_image, radius, sigma, angle);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = MotionBlurImage(Image_val(_image),
               Double_val(radius), Double_val(sigma), Double_val(angle), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.motion_blur");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_EmbossImage(value _image, value radius, value sigma)
{
    CAMLparam3(_image, radius, sigma);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = EmbossImage(Image_val(_image), Double_val(radius), Double_val(sigma), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.emboss");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_SharpenImage(value _image, value radius, value sigma)
{
    CAMLparam3(_image, radius, sigma);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = SharpenImage(Image_val(_image), Double_val(radius), Double_val(sigma), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.sharpen");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_ShadeImage(value _image, value gray, value azimuth, value elevation)
{
    CAMLparam4(_image, gray, azimuth, elevation);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = ShadeImage(Image_val(_image), Int_val(gray), Double_val(azimuth), Double_val(elevation), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.shade");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_EdgeImage(value _image, value radius)
{
    CAMLparam2(_image, radius);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = EdgeImage(Image_val(_image), Double_val(radius), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.edge");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_EnhanceImage(value _image)
{
    CAMLparam1(_image);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = EnhanceImage(Image_val(_image), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.enhance");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_ContrastImage(value image, value sharpen)
{
    ContrastImage(Image_val(image), Int_val(sharpen));
    return Val_unit;
}

CAMLprim value
caml_EqualizeImage(value image)
{
    EqualizeImage(Image_val(image));
    return Val_unit;
}

CAMLprim value
caml_GammaImage(value image, value level)
{
    GammaImage(Image_val(image), String_val(level));
    return Val_unit;
}

CAMLprim value
caml_LevelImage(value image, value level)
{
    LevelImage(Image_val(image), String_val(level));
    return Val_unit;
}

static const ChannelType channeltype_table[] = {
  UndefinedChannel,
  RedChannel,
  CyanChannel,
  GreenChannel,
  MagentaChannel,
  BlueChannel,
  YellowChannel,
  OpacityChannel,
  BlackChannel,
  MatteChannel,
  AllChannels,
  GrayChannel
};

#define ChannelType_val(i) (channeltype_table[Int_val(i)])

CAMLprim value
caml_LevelImageChannel(value image, value channel,
        value black_point, value mid_point, value white_point)
{
    LevelImageChannel(
            Image_val(image),
            ChannelType_val(channel),
            Double_val(black_point),
            Double_val(mid_point),
            Double_val(white_point) );

    return Val_unit;
}

CAMLprim value
caml_ModulateImage(value image, value modulate)
{
    ModulateImage(Image_val(image), String_val(modulate));
    return Val_unit;
}

CAMLprim value
caml_NegateImage(value image, value grayscale)
{
    NegateImage(Image_val(image), Int_val(grayscale));
    return Val_unit;
}

CAMLprim value
caml_NormalizeImage(value image)
{
    NormalizeImage(Image_val(image));
    return Val_unit;
}

CAMLprim value
caml_FlipImage(value _image)
{
    CAMLparam1(_image);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = FlipImage(Image_val(_image), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.flip");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_FlopImage(value _image)
{
    CAMLparam1(_image);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = FlopImage(Image_val(_image), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.flop");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_CropImage(value _image, value _geometry)
{
    CAMLparam2(_image, _geometry);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    RectangleInfo geometry;
    geometry.x      = Int_val(Field(_geometry, 0));
    geometry.y      = Int_val(Field(_geometry, 1));
    geometry.width  = Int_val(Field(_geometry, 2));
    geometry.height = Int_val(Field(_geometry, 3));

    Image *image = (Image *) NULL;
    image = CropImage(Image_val(_image), &geometry, &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.crop");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_ShaveImage(value _image, value _shave_info)
{
    CAMLparam2(_image, _shave_info);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    RectangleInfo shave_info;
    shave_info.x      = Int_val(Field(_shave_info, 0));
    shave_info.y      = Int_val(Field(_shave_info, 1));
    shave_info.width  = Int_val(Field(_shave_info, 2));
    shave_info.height = Int_val(Field(_shave_info, 3));

    Image *image = (Image *) NULL;
    image = ShaveImage(Image_val(_image), &shave_info, &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.shave");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_MagnifyImage(value _image)
{
    CAMLparam1(_image);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = MagnifyImage(Image_val(_image), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.magnify");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_MinifyImage(value _image)
{
    CAMLparam1(_image);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = MinifyImage(Image_val(_image), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.minify");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_SampleImage(value _image, value width, value height)
{
    CAMLparam3(_image, width, height);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = SampleImage(Image_val(_image), Int_val(width), Int_val(height), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.sample");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_ScaleImage(value _image, value width, value height)
{
    CAMLparam3(_image, width, height);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = ScaleImage(Image_val(_image), Int_val(width), Int_val(height), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.scale");
    }
    CAMLreturn(Val_Image(image));
}

CAMLprim value
caml_ThumbnailImage(value _image, value width, value height)
{
    CAMLparam3(_image, width, height);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    Image *image = (Image *) NULL;
    image = ThumbnailImage(Image_val(_image), Int_val(width), Int_val(height), &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.thumbnail");
    }
    CAMLreturn(Val_Image(image));
}

static const FilterTypes filtertypes_table[] = {
  UndefinedFilter,
  DefaultResizeFilter,
  PointFilter,
  BoxFilter,
  TriangleFilter,
  HermiteFilter,
  HanningFilter,
  HammingFilter,
  BlackmanFilter,
  GaussianFilter,
  QuadraticFilter,
  CubicFilter,
  CatromFilter,
  MitchellFilter,
  LanczosFilter,
  BesselFilter,
  SincFilter
};

CAMLprim value
caml_ResizeImage(value _image, value width, value height, value _filter, value _blur)
{
    CAMLparam5(_image, width, height, _filter, _blur);

    ExceptionInfo exception;
    GetExceptionInfo(&exception);

    FilterTypes filter = filtertypes_table[Int_val(_filter)];
    double blur = Double_val(_blur);

    Image *image = (Image *) NULL;
    image = ResizeImage(Image_val(_image), Int_val(width), Int_val(height),
                 filter, blur, &exception);

    if (image == (Image *) NULL)
    {
        CatchException(&exception);
        caml_failwith("Magick.resize");
    }
    CAMLreturn(Val_Image(image));
}

static const CompositeOperator  compositeoperator_table[] = {
  UndefinedCompositeOp,
  OverCompositeOp,
  InCompositeOp,
  OutCompositeOp,
  AtopCompositeOp,
  XorCompositeOp,
  PlusCompositeOp,
  MinusCompositeOp,
  AddCompositeOp,
  SubtractCompositeOp,
  DifferenceCompositeOp,
  MultiplyCompositeOp,
  BumpmapCompositeOp,
  CopyCompositeOp,
  CopyRedCompositeOp,
  CopyGreenCompositeOp,
  CopyBlueCompositeOp,
  CopyOpacityCompositeOp,
  ClearCompositeOp,
  DissolveCompositeOp,
  DisplaceCompositeOp,
  ModulateCompositeOp,
  ThresholdCompositeOp,
  NoCompositeOp,
  DarkenCompositeOp,
  LightenCompositeOp,
  HueCompositeOp,
  SaturateCompositeOp,
  ColorizeCompositeOp,
  LuminizeCompositeOp,
  ScreenCompositeOp,
  OverlayCompositeOp,
  CopyCyanCompositeOp,
  CopyMagentaCompositeOp,
  CopyYellowCompositeOp,
  CopyBlackCompositeOp,
  DivideCompositeOp,
  HardLightCompositeOp,
  ExclusionCompositeOp,
  ColorDodgeCompositeOp,
  ColorBurnCompositeOp,
  SoftLightCompositeOp,
  LinearBurnCompositeOp,
  LinearDodgeCompositeOp,
  LinearLightCompositeOp,
  VividLightCompositeOp,
  PinLightCompositeOp,
  HardMixCompositeOp
};

CAMLprim value
caml_CompositeImage(value _image1, value compOp, value _image2, value x_offset, value y_offset)
{
    CAMLparam5(_image1, compOp, _image2, x_offset, y_offset);

    CompositeOperator compose = compositeoperator_table[Int_val(compOp)];

    if (!CompositeImage(Image_val(_image1), compose, Image_val(_image2),
                 Int_val(x_offset), Int_val(y_offset)))
    {
        caml_failwith("Magick.composite");
    }
    CAMLreturn(Val_unit);
}

/* Image List */

CAMLprim value
caml_NewImageList(value unit)
{
    Image *image = NewImageList();
    return Val_Image(image);
}

CAMLprim value
caml_DestroyImageList(value image)
{
    DestroyImageList(Image_val(image));
    return Val_unit;
}

CAMLprim value
caml_AppendImageToList(value _images, value image)
{
    Image * images = Image_val(_images);
    AppendImageToList(&images, Image_val(image));
    return Val_Image(images);
}

CAMLprim value
caml_PrependImageToList(value _images, value image)
{
    Image * images = Image_val(_images);
    PrependImageToList(&images, Image_val(image));
    return Val_Image(images);
}

CAMLprim value
caml_GetFirstImageInList(value imageList)
{
    Image *image = GetFirstImageInList(Image_val(imageList));
    return Val_Image(image);
}

CAMLprim value
caml_GetImageFromList(value imageList, value offset)
{
    Image *image = GetImageFromList(Image_val(imageList), Long_val(offset));
    return Val_Image(image);
}

CAMLprim value
caml_GetImageListLength(value images)
{
    unsigned long length = GetImageListLength(Image_val(images));
    return Val_long(length);
}

CAMLprim value
caml_GetImageIndexInList(value images)
{
    long index = GetImageIndexInList(Image_val(images));
    return Val_long(index);
}

/* <magick/draw.h> */

static value Val_DrawContext(DrawContext ctx)
{
    value v = caml_alloc(1, Abstract_tag);
    *((DrawContext *) Data_abstract_val(v)) = ctx;
    return v;
}

static DrawContext DrawContext_val(value v)
{
    return *((DrawContext *) Data_abstract_val(v));
}

CAMLprim value
caml_DrawAllocateContext(value image)
{
    DrawInfo *draw_info = NULL;

    DrawContext ctx =
        DrawAllocateContext(draw_info, Image_val(image));

    return Val_DrawContext(ctx);
}

CAMLprim value
caml_DrawSetStrokeColorString(value context, value stroke_color)
{
    DrawSetStrokeColorString(DrawContext_val(context), String_val(stroke_color));
    return Val_unit;
}

CAMLprim value
caml_DrawSetFillColorString(value context, value fill_color)
{
    DrawSetFillColorString(DrawContext_val(context), String_val(fill_color));
    return Val_unit;
}

CAMLprim value
caml_DrawSetStrokeWidth(value context, value width)
{
    DrawSetStrokeWidth(DrawContext_val(context), Double_val(width));
    return Val_unit;
}

CAMLprim value
caml_DrawCircle(value context, value ox, value oy, value px, value py)
{
    DrawCircle(
            DrawContext_val(context),
            Double_val(ox), Double_val(oy),
            Double_val(px), Double_val(py));

    return Val_unit;
}

CAMLprim value
caml_DrawRectangle(value context, value x1, value y1, value x2, value y2)
{
    DrawRectangle(
            DrawContext_val(context),
            Double_val(x1), Double_val(y1),
            Double_val(x2), Double_val(y2));

    return Val_unit;
}

CAMLprim value
caml_DrawRender(value context)
{
    DrawRender(DrawContext_val(context));
    return Val_unit;
}

/* vim: sw=4 ts=4 sts=4 et
 */
