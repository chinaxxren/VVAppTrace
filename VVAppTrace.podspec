
Pod::Spec.new do |spec|
  spec.name         = "VVAppTrace"
  spec.version      = "0.0.1"
  spec.summary      = "VVAppTrace 监控ios方法执行性能工具"
  spec.homepage     = "https://github.com/chinaxxren/VVAppTrace"
  spec.license      = "MIT"
  spec.author       = { "chinaxxren" => "182421693@qq.com" }
  spec.platform     = :ios, "9.0"
  spec.source       = { :git => "https://github.com/chinaxxren/VVAppTrace.git", :tag => "#{spec.version}" }
  spec.source_files  = "VVAppTrace/Source", "VVAppTrace/Source/**/*.*"
  spec.frameworks  = "UIKit"
end
