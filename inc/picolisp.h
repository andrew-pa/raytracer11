#pragma once

#include "picojson.h"

namespace picojson {
	struct lisp_functor {

		value body;
		vector<string> arg_names;

		lisp_functor(value b, vector<string> an) : body(b), arg_names(an) {}

		value operator()(context& cx2, const std::vector<value>& args) {
			cx2.scopes.push_front({});
			for (int i = 0; i < args.size() && i < arg_names.size(); ++i) {
				cx2.name_value(arg_names[i], args[i]);
			}
			auto res = resolve(body, cx2);
			cx2.scopes.pop_front();
			return res;
		}
	};

	void init_cmmn_lisp(context& cx) {
		cx.name_value("+", value(vfunc([](context& cx, const std::vector<value>& args) { 
			double v = 0.;
			for (const auto& a : args) v += resolve(a,cx).get<double>();
			return value(v);
		})));
		cx.name_value("-", value(vfunc([](context& cx, const std::vector<value>& args) {
			double v = resolve(args[0], cx).get<double>();
			for (int i = 1; i < args.size(); ++i) v -= resolve(args[i], cx).get<double>();
			return value(v);
		})));
		cx.name_value("*", value(vfunc([](context& cx, const std::vector<value>& args) {
			double v = resolve(args[0], cx).get<double>();
			for (int i = 1; i < args.size(); ++i) v *= resolve(args[i], cx).get<double>();
			return value(v);
		})));
		cx.name_value("/", value(vfunc([](context& cx, const std::vector<value>& args) {
			return value(resolve(args[0], cx).get<double>()/ resolve(args[1], cx).get<double>());
		})));
		cx.name_value("defvar", value(vfunc([](context& cx, const std::vector<value>& args) {
			cx.name_value(args[0].get<std::string>(), resolve(args[1], cx));
			return value(null_type,false);
		})));
		cx.name_value("defun", value(vfunc([](context& cx, const std::vector<value>& args) {
			//(defun *name* (*args*) (*body*)) 
			//(defun madd (x y z) (+ (* x y) z))
			vector<string> ans;
			auto nx = &args[1].get<cons_cell>();
			int i = 0;
			while (nx != nullptr) {
				if (nx->first == nullptr) break;
				ans.push_back(nx->first->get<std::string>());
				if (nx->last == nullptr) break;
				nx = &nx->last->get<cons_cell>();
			}
			cx.name_value(args[0].get<std::string>(), value(vfunc(lisp_functor(args[2], ans))));
			return value(null_type, false);
		})));
	}
}