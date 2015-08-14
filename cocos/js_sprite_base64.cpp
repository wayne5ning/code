/****************************************************************************
Copyright (c) 2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "cocos2d.h"
#include "cocos2d_specifics.hpp"

using namespace cocos2d;


namespace {

	class SpriteBase64 : public Ref
	{
	public:
		static Sprite* create(const std::string& str);
	};

	Sprite* SpriteBase64::create(const std::string& str)
	{
		int len = 0;
		unsigned char *buffer;
		len = base64Decode((unsigned char*)str.c_str(), (unsigned int)str.length(), &buffer);

		Image *img = new Image();
		bool ok = img->initWithImageData(buffer, len);

		if (!ok) {
			log("ERROR CREATING IMAGE WITH BASE64");
			return nullptr;
		}
		Texture2D* texture = new Texture2D();
		texture->initWithImage(img);
		Sprite* sprite = new Sprite();
		if (sprite && sprite->initWithTexture(texture)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	static bool js_is_native_obj(JSContext *cx, uint32_t argc, jsval *vp)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		args.rval().setBoolean(true);
		return true;
	}

	JSClass  *jsb_SpriteBase64_class;
	JSObject *jsb_SpriteBase64_prototype;

	bool js_cocos2dx_SpriteBase64_create(JSContext *cx, uint32_t argc, jsval *vp)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		if (argc == 1) {
			std::string arg0;
			bool ok = true;
			ok &= jsval_to_std_string(cx, args.get(0), &arg0);
			JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_SpriteBase64_create : Error processing arguments");
			Sprite* ret = SpriteBase64::create(arg0);
			jsval jsret = JSVAL_NULL;
			do {
				if (ret) {
					js_proxy_t *jsProxy = js_get_or_create_proxy<Sprite>(cx, (Sprite*)ret);
					jsret = OBJECT_TO_JSVAL(jsProxy->obj);
				}
				else {
					jsret = JSVAL_NULL;
				}
			} while (0);
			args.rval().set(jsret);
			return true;
		}
		JS_ReportError(cx, "js_cocos2dx_SpriteBase64_create : wrong number of arguments");
		return false;
	}

	void js_SpriteBase64_finalize(JSFreeOp *fop, JSObject *obj) {
		CCLOGINFO("jsbindings: finalizing JS object %p (SpriteBase64)", obj);
	}

	static bool empty_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
		return false;
	}

	void js_register_cocos2dx_SpriteBase64(JSContext *cx, JS::HandleObject global) {
		jsb_SpriteBase64_class = (JSClass *)calloc(1, sizeof(JSClass));
		jsb_SpriteBase64_class->name = "SpriteBase64";
		jsb_SpriteBase64_class->addProperty = JS_PropertyStub;
		jsb_SpriteBase64_class->delProperty = JS_DeletePropertyStub;
		jsb_SpriteBase64_class->getProperty = JS_PropertyStub;
		jsb_SpriteBase64_class->setProperty = JS_StrictPropertyStub;
		jsb_SpriteBase64_class->enumerate = JS_EnumerateStub;
		jsb_SpriteBase64_class->resolve = JS_ResolveStub;
		jsb_SpriteBase64_class->convert = JS_ConvertStub;
		jsb_SpriteBase64_class->finalize = js_SpriteBase64_finalize;
		jsb_SpriteBase64_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

		static JSPropertySpec properties[] = {
			JS_PSG("__nativeObj", js_is_native_obj, JSPROP_PERMANENT | JSPROP_ENUMERATE),
			JS_PS_END
		};

		static JSFunctionSpec* funcs = nullptr;

		static JSFunctionSpec st_funcs[] = {
			JS_FN("create", js_cocos2dx_SpriteBase64_create, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
			JS_FS_END
		};

		jsb_SpriteBase64_prototype = JS_InitClass(
			cx, global,
			JS::NullPtr(),
			jsb_SpriteBase64_class,
			empty_constructor, 0,
			properties,
			funcs,
			NULL, // no static properties
			st_funcs);
		// make the class enumerable in the registered namespace
		//  bool found;
		//FIXME: Removed in Firefox v27
		//  JS_SetPropertyAttributes(cx, global, "SpriteBase64", JSPROP_ENUMERATE | JSPROP_READONLY, &found);

		// add the proto and JSClass to the type->js info hash table
		TypeTest<SpriteBase64> t;
		js_type_class_t *p;
		std::string typeName = t.s_name();
		if (_js_global_type_map.find(typeName) == _js_global_type_map.end())
		{
			p = (js_type_class_t *)malloc(sizeof(js_type_class_t));
			p->jsclass = jsb_SpriteBase64_class;
			p->proto = jsb_SpriteBase64_prototype;
			p->parentProto = NULL;
			_js_global_type_map.insert(std::make_pair(typeName, p));
		}
	}
}

void register_sprite_base64_bindings(JSContext *cx, JS::HandleObject global)
{
	JS::RootedObject ccobj(cx);
	get_or_create_js_obj(cx, global, "cc", &ccobj);
	js_register_cocos2dx_SpriteBase64(cx, ccobj);
}